# CRTP 方案解决 NodeWrapper 劫持问题

## 问题回顾

### 当前架构的问题

```
ComponentWrapper::Initialize()
    │
    ├── wrapper_node_.reset(new NodeWrapper(config.name()))
    │
    └── GetNode() 返回 shared_ptr<Node>
              │
              ▼
        shared_ptr<NodeWrapper> → shared_ptr<Node> (隐式转换)
              │
              ▼
        this->GetNode()->CreateReader<Msg>(...)
              │
              ▼
        编译器根据 shared_ptr<Node> 绑定到 Node::CreateReader
              │
              ▼
        ❌ 绕过了 NodeWrapper::CreateReader() 劫持逻辑！
```

### 根本原因

`CreateReader` 是**模板函数**，不是**虚函数**：

| 函数类型 | 绑定时机 | 查找方式 |
|---------|---------|---------|
| 虚函数 | 运行时 | 通过 vtable 查找实际对象的方法 |
| 模板函数 | 编译时 | 根据**指针类型**绑定，忽略实际对象 |

当执行 `shared_ptr<Node>→CreateReader<Message>()` 时：
- 编译器只看到 `shared_ptr<Node>` 类型
- 绑定到 `Node::CreateReader<Message>()`
- 完全没有调用 `NodeWrapper::CreateReader<Message>()`

---

## 方案五：CRTP（奇异递归模板模式）+ INode 接口

### 核心思想

**编译时多态**：通过模板参数传递派生类类型，在编译期决定调用哪个方法，而不是运行时通过虚函数表。

同时引入 **INode 接口**，作为所有节点类型的统一抽象层。

### 传统继承 vs CRTP

#### 传统继承（运行时多态）

```cpp
class Base {
public:
    virtual void method() = 0;
};

class Derived : public Base {
public:
    void method() override {
        std::cout << "Derived method" << std::endl;
    }
};

// 调用
Base* ptr = new Derived();
ptr->method();  // 运行时通过 vtable 找到 Derived::method()
```

#### CRTP（编译时多态）

```cpp
// 基类模板，接收派生类类型作为模板参数
template <typename Derived>
class Base {
public:
    // 调用时会转发到 Derived 的实现
    void method() {
        static_cast<Derived*>(this)->implementation();
    }
};

// 派生类
class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation" << std::endl;
    }
};

// 调用
Derived d;
d.method();  // 编译时绑定到 Derived::implementation()
```

### CRTP + INode 解决劫持问题的思路

#### 关键洞察

原来的问题是：
```
GetNode() 返回 shared_ptr<Node>
→ 编译器绑定 Node::CreateReader
→ 绕过 NodeWrapper::CreateReader
```

CRTP + INode 的思路是：
1. **INode** 作为统一的接口层，所有节点类型都继承它
2. **CRTP 模板**实现编译时多态，转发到具体的 `*_impl` 方法
3. **RealNode** 和 **NodeWrapper** 都通过 INode 暴露统一的 `CreateReader` 接口
4. 用户代码通过 `shared_ptr<INode>` 调用，运行时根据实际类型调用对应的 `*_impl` 方法

#### 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                        INode (接口基类)                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  template CreateReader() {                             │  │
│  │    return derived()->CreateReader_impl(...);           │  │
│  │  }                                                    │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                         ▲
                         │ 继承 (CRTP)
          ┌──────────────┴──────────────┐
          │                              │
┌─────────┴─────────┐         ┌─────────┴─────────┐
│     RealNode      │         │    NodeWrapper    │
│                   │         │                   │
│ CreateReader_impl│         │ CreateReader_impl │
│   ↓               │         │   ↓               │
│ 真正的实现        │         │ 劫持 + 调用       │
│                   │         │ RealNode          │
└───────────────────┘         └───────────────────┘
```

---

## 具体实现方案

### 方案 A：INode + RealNode + NodeWrapper

#### 1. INode - 接口基类

```cpp
// cyber/node/inode.h
#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <map>

#include "cyber/cyber.h"
#include "cyber/node/node_channel_impl.h"
#include "cyber/node/node_service_impl.h"
#include "cyber/reader.h"
#include "cyber/writer.h"

namespace apollo {
namespace cyber {

/**
 * @class INode
 * @brief 节点接口基类，使用 CRTP 模式实现编译时多态
 *
 * @tparam Derived 派生类类型，用于 CRTP 转发
 *
 * 所有节点类型（RealNode、NodeWrapper）都必须继承此类，
 * 并实现 CreateReader_impl、CreateWriter_impl 等方法。
 */
template <typename Derived>
class INode {
protected:
    // CRTP 核心：返回自身类型
    Derived* self() { return static_cast<Derived*>(this); }
    const Derived* self() const { return static_cast<const Derived*>(this); }

    std::string node_name_;
    std::string name_space_;

    std::mutex readers_mutex_;
    std::map<std::string, std::shared_ptr<ReaderBase>> readers_;

    std::unique_ptr<NodeChannelImpl> node_channel_impl_ = nullptr;
    std::unique_ptr<NodeServiceImpl> node_service_impl_ = nullptr;

public:
    virtual ~INode() = default;

    const std::string& Name() const { return node_name_; }

    // ========== CreateWriter 模板（转发到 *_impl）==========
    template <typename MessageT>
    auto CreateWriter(const proto::RoleAttributes& role_attr)
        -> std::shared_ptr<Writer<MessageT>> {
        return self()->template CreateWriter_impl<MessageT>(role_attr);
    }

    template <typename MessageT>
    auto CreateWriter(const std::string& channel_name)
        -> std::shared_ptr<Writer<MessageT>> {
        return self()->template CreateWriter_impl<MessageT>(channel_name);
    }

    // ========== CreateReader 模板（转发到 *_impl）==========
    // 所有 CreateReader 调用都通过这些模板方法
    // 转发到 Derived::CreateReader_impl()

    template <typename MessageT>
    auto CreateReader(const std::string& channel_name,
                      const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        return self()->template CreateReader_impl<MessageT>(
            channel_name, reader_func);
    }

    template <typename MessageT>
    auto CreateReader(const ReaderConfig& config,
                      const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        return self()->template CreateReader_impl<MessageT>(config, reader_func);
    }

    template <typename MessageT>
    auto CreateReader(const proto::RoleAttributes& role_attr,
                      const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        return self()->template CreateReader_impl<MessageT>(role_attr, reader_func);
    }

    // ========== CreateService 模板 ==========
    template <typename Request, typename Response>
    auto CreateService(const std::string& service_name,
                       const typename Service<Request, Response>::ServiceCallback&
                           service_callback)
        -> std::shared_ptr<Service<Request, Response>> {
        return node_service_impl_->template CreateService<Request, Response>(
            service_name, service_callback);
    }

    // ========== CreateClient 模板 ==========
    template <typename Request, typename Response>
    auto CreateClient(const std::string& service_name)
        -> std::shared_ptr<Client<Request, Response>> {
        return node_service_impl_->template CreateClient<Request, Response>(
            service_name);
    }

    // ========== GetReader 模板 ==========
    template <typename MessageT>
    auto GetReader(const std::string& name)
        -> std::shared_ptr<Reader<MessageT>> {
        std::lock_guard<std::mutex> lg(readers_mutex_);
        auto it = readers_.find(name);
        if (it != readers_.end()) {
            return std::dynamic_pointer_cast<Reader<MessageT>>(it->second);
        }
        return nullptr;
    }

    void Observe() { /* ... */ }
    void ClearData() { /* ... */ }
};

} // namespace cyber
} // namespace apollo
```

#### 2. RealNode - 真正的 Node 实现

```cpp
// cyber/node/real_node.h
#pragma once

#include "cyber/node/inode.h"

namespace apollo {
namespace cyber {

/**
 * @class RealNode
 * @brief 真正的 Node 实现，所有方法都是标准实现
 *
 * 继承自 INode<RealNode>，使用 CRTP 模式。
 * 提供真正的 CreateReader_impl、CreateWriter_impl 实现。
 */
class RealNode : public INode<RealNode> {
public:
    RealNode(const std::string& node_name, const std::string& name_space = "") {
        node_name_ = node_name;
        name_space_ = name_space;
        node_channel_impl_.reset(new NodeChannelImpl(node_name));
        node_service_impl_.reset(new NodeServiceImpl(node_name));
    }

    ~RealNode() = default;

    // ========== CreateReader_impl 实现（标准实现）==========
    template <typename MessageT>
    auto CreateReader_impl(const std::string& channel_name,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        std::lock_guard<std::mutex> lg(readers_mutex_);
        if (readers_.find(channel_name) != readers_.end()) {
            AWARN << "Reader with same channel already exists.";
            return nullptr;
        }
        auto reader = node_channel_impl_->template CreateReader<MessageT>(
            channel_name, reader_func);
        if (reader != nullptr) {
            readers_.emplace(std::make_pair(channel_name, reader));
        }
        return reader;
    }

    template <typename MessageT>
    auto CreateReader_impl(const ReaderConfig& config,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        std::lock_guard<std::mutex> lg(readers_mutex_);
        if (readers_.find(config.channel_name) != readers_.end()) {
            AWARN << "Reader with same channel already exists.";
            return nullptr;
        }
        auto reader = node_channel_impl_->template CreateReader<MessageT>(
            config, reader_func);
        if (reader != nullptr) {
            readers_.emplace(std::make_pair(config.channel_name, reader));
        }
        return reader;
    }

    template <typename MessageT>
    auto CreateReader_impl(const proto::RoleAttributes& role_attr,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        std::lock_guard<std::mutex> lg(readers_mutex_);
        if (readers_.find(role_attr.channel_name()) != readers_.end()) {
            AWARN << "Reader with same channel already exists.";
            return nullptr;
        }
        auto reader = node_channel_impl_->template CreateReader<MessageT>(
            role_attr, reader_func);
        if (reader != nullptr) {
            readers_.emplace(std::make_pair(role_attr.channel_name(), reader));
        }
        return reader;
    }

    // ========== CreateWriter_impl 实现（标准实现）==========
    template <typename MessageT>
    auto CreateWriter_impl(const std::string& channel_name)
        -> std::shared_ptr<Writer<MessageT>> {
        return node_channel_impl_->template CreateWriter<MessageT>(channel_name);
    }

    template <typename MessageT>
    auto CreateWriter_impl(const proto::RoleAttributes& role_attr)
        -> std::shared_ptr<Writer<MessageT>> {
        return node_channel_impl_->template CreateWriter<MessageT>(role_attr);
    }
};

} // namespace cyber
} // namespace apollo
```

#### 3. NodeWrapper - 带劫持的包装器

```cpp
// cyber/node/node_wrapper.h
#pragma once

#include <memory>
#include <string>

#include "cyber/node/inode.h"
#include "cyber/node/real_node.h"

namespace apollo {
namespace cyber {

/**
 * @class NodeWrapper
 * @brief 劫持 Node 的关键方法，用于调试和监控
 *
 * 继承自 INode<NodeWrapper>，使用 CRTP 模式。
 * 所有 CreateReader/CreateWriter 调用都会先进入 *_impl 方法，
 * 在这里可以添加劫持逻辑（如打印日志），然后转发到 RealNode。
 */
class NodeWrapper : public INode<NodeWrapper> {
private:
    // 组合：持有真正的 Node 实现
    std::shared_ptr<RealNode> real_node_;

public:
    explicit NodeWrapper(const std::string& node_name,
                        const std::string& name_space = "") {
        // 创建真正的 Node
        real_node_ = std::make_shared<RealNode>(node_name, name_space);

        // 共享 INode 的成员变量（通过移动或引用共享）
        // 注意：这里需要特殊处理，因为 INode 的成员是 protected
        // 更好的做法是让 INode 提供共享底层实现的接口

        fprintf(stderr, "NodeWrapper created for: %s\n", node_name.c_str());
        fflush(stderr);
    }

    ~NodeWrapper() {
        fprintf(stderr, "NodeWrapper destroyed\n");
        fflush(stderr);
    }

    // ========== 劫持实现 ==========
    // 所有 CreateReader 调用都会先进入这里

    template <typename MessageT>
    auto CreateReader_impl(const std::string& channel_name,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        // 劫持逻辑：打印日志
        fprintf(stderr, "NodeWrapper::CreateReader_impl: %s\n", channel_name.c_str());
        fflush(stderr);

        // 转发到真正的 Node 实现
        return real_node_->CreateReader_impl<MessageT>(channel_name, reader_func);
    }

    template <typename MessageT>
    auto CreateReader_impl(const ReaderConfig& config,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        fprintf(stderr, "NodeWrapper::CreateReader_impl (ReaderConfig): %s\n",
                config.channel_name.c_str());
        fflush(stderr);
        return real_node_->CreateReader_impl<MessageT>(config, reader_func);
    }

    template <typename MessageT>
    auto CreateReader_impl(const proto::RoleAttributes& role_attr,
                          const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        fprintf(stderr, "NodeWrapper::CreateReader_impl (RoleAttr): %s\n",
                role_attr.channel_name().c_str());
        fflush(stderr);
        return real_node_->CreateReader_impl<MessageT>(role_attr, reader_func);
    }

    // ========== 劫持 CreateWriter ==========
    template <typename MessageT>
    auto CreateWriter_impl(const std::string& channel_name)
        -> std::shared_ptr<Writer<MessageT>> {
        fprintf(stderr, "NodeWrapper::CreateWriter_impl: %s\n", channel_name.c_str());
        fflush(stderr);
        return real_node_->CreateWriter_impl<MessageT>(channel_name);
    }

    template <typename MessageT>
    auto CreateWriter_impl(const proto::RoleAttributes& role_attr)
        -> std::shared_ptr<Writer<MessageT>> {
        fprintf(stderr, "NodeWrapper::CreateWriter_impl (RoleAttr): %s\n",
                role_attr.channel_name().c_str());
        fflush(stderr);
        return real_node_->CreateWriter_impl<MessageT>(role_attr);
    }
};

} // namespace cyber
} // namespace apollo
```

#### 4. ComponentWrapper 修改

```cpp
// cyber/component/component_wrapper.h
#pragma once

#include "cyber/component/component.h"
#include "cyber/component/component_base.h"
#include "cyber/node/node_wrapper.h"
#include "cyber/node/real_node.h"

namespace apollo {
namespace cyber {

template <typename M0 = NullType, typename M1 = NullType,
          typename M2 = NullType, typename M3 = NullType>
class ComponentWrapper : public Component<M0, M1, M2, M3> {
public:
    ComponentWrapper() {}
    virtual ~ComponentWrapper() {}

    bool Initialize(const ComponentConfig &config) override {
        printf("ComponentWrapper Initialize file:%s:%d\n", __FILE__, __LINE__);

        if (use_wrapper_) {
            // 使用 NodeWrapper（带劫持）
            // shared_ptr<NodeWrapper> 继承自 INode<NodeWrapper>
            // CRTP 会正确转发到 NodeWrapper::CreateReader_impl
            this->wrapper_node_.reset(new NodeWrapper(config.name()));
        } else {
            // 使用真正的 Node
            this->real_node_.reset(new RealNode(config.name()));
            // CRTP 会正确转发到 RealNode::CreateReader_impl
        }

        if (!this->Init()) {
            AERROR << "Component Init() failed.";
            return false;
        }

        return true;
    }

protected:
    bool use_wrapper_ = true;

    // 方案 1：使用 std::variant（C++17）
    std::variant<
        std::shared_ptr<NodeWrapper>,
        std::shared_ptr<RealNode>
    > node_holder_;

    // 方案 2：使用模板参数统一类型
    // template <typename NodeImpl>
    // std::shared_ptr<INode<NodeImpl>> node_;

    // 方案 3：使用接口指针
    // INode* node_base_ = nullptr;

    // 获取节点指针
    // 注意：返回类型需要统一，可以通过以下方式之一：
    // 1. 返回 std::variant（需要用户处理）
    // 2. 返回 shared_ptr<INode<...>>（CRTP 基类）
    // 3. 提供统一的访问接口
};

} // namespace cyber
} // namespace apollo
```

---

### 方案 B：虚函数 + 模板转发（混合方案）

如果不想大规模重构 Node 类，可以使用混合方案：

```cpp
// cyber/node/inode.h (简化为接口)
class INode {
public:
    virtual ~INode() {}

    // 虚函数接口 - 用于运行时多态
    virtual std::shared_ptr<ReaderBase> CreateReaderBase(
        const std::string& channel_name) = 0;

    // 模板方法 - 转发到虚函数
    template <typename MessageT>
    auto CreateReader(const std::string& channel_name,
                      const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<cyber::Reader<MessageT>> {
        // 先执行劫持逻辑
        OnBeforeCreateReader<MessageT>(channel_name);

        // 调用虚函数
        auto base_reader = CreateReaderBase(channel_name);

        // 后置处理
        OnAfterCreateReader<MessageT>(base_reader);

        return std::dynamic_pointer_cast<cyber::Reader<MessageT>>(base_reader);
    }

protected:
    // 劫持钩子 - 可被子类重写
    template <typename MessageT>
    void OnBeforeCreateReader(const std::string& channel_name) {}

    template <typename MessageT>
    void OnAfterCreateReader(std::shared_ptr<ReaderBase> reader) {}
};

// RealNode
class RealNode : public INode {
public:
    std::shared_ptr<ReaderBase> CreateReaderBase(
        const std::string& channel_name) override {
        // 真正的实现
    }
};

// NodeWrapper
class NodeWrapper : public INode {
public:
    std::shared_ptr<ReaderBase> CreateReaderBase(
        const std::string& channel_name) override {
        fprintf(stderr, "NodeWrapper: %s\n", channel_name.c_str());
        return real_node_->CreateReaderBase(channel_name);
    }

    template <typename MessageT>
    void OnBeforeCreateReader(const std::string& channel_name) override {
        fprintf(stderr, "NodeWrapper hook before: %s\n", channel_name.c_str());
    }
};
```

---

## 调用流程对比

### 原来的流程（有问题）

```
用户代码: this->GetNode()->CreateReader<Msg>("channel")
                              ↓
                      GetNode() 返回 shared_ptr<Node>
                              ↓
                      隐式转换为 shared_ptr<NodeWrapper>
                              ↓
                      shared_ptr<Node>→CreateReader<Msg>()
                              ↓
                编译器绑定: Node::CreateReader<Msg>()
                              ↓
                      ❌ 绕过 NodeWrapper
```

### CRTP + INode 方案（正确）

```
用户代码: this->GetNode()->CreateReader<Msg>("channel")
                              ↓
                      GetNode() 返回 shared_ptr<NodeWrapper>
                              ↓
                      shared_ptr<NodeWrapper>→CreateReader<Msg>()
                              ↓
                INode<NodeWrapper>::CreateReader<Msg>()
                （编译时通过 CRTP 转发）
                              ↓
                static_cast<NodeWrapper*>(this)
                              ↓
                NodeWrapper::CreateReader_impl<Msg>()
                              ↓
                      ✅ 劫持逻辑执行
                              ↓
                real_node_->CreateReader_impl<Msg>()
                              ↓
                      真正的实现
```

---

## CRTP + INode 方案优缺点

### 优点

| 优点 | 说明 |
|-----|------|
| **劫持透明** | 用户代码不用改，调用方式不变 |
| **零运行时开销** | 编译时解析，无 vtable 查找 |
| **统一接口** | RealNode 和 NodeWrapper 都继承 INode |
| **可组合** | 可以叠加多个 Mixin（日志、监控、重试...） |
| **类型安全** | 编译器检查，比宏更安全 |

### 缺点

| 缺点 | 说明 |
|-----|------|
| **改动大** | 需要重构 Node 类结构 |
| **复杂度高** | 模板代码阅读困难 |
| **编译时间长** | 模板实例化增加编译时间 |
| **调试困难** | 模板错误信息可能很长 |

---

## 其他可选方案

### 方案一：改 GetNode() 返回类型

```cpp
// component_wrapper.h
std::shared_ptr<NodeWrapper> GetNode() {
    return wrapper_node_;
}
```

**优点**：改动最小
**缺点**：接口类型变了

### 方案二：ComponentWrapper 手动转发

```cpp
template <typename MessageT>
auto CreateReader(const std::string& channel_name,
                 const CallbackFunc<MessageT>& reader_func = nullptr)
    -> std::shared_ptr<cyber::Reader<MessageT>> {
    return wrapper_node_->CreateReader<MessageT>(channel_name, reader_func);
}
```

**优点**：劫持透明
**缺点**：需要手动转发所有方法

### 方案三：改用户代码调用方式

```cpp
// 用户直接用 wrapper_node_
wrapper_node_->CreateReader<Msg>(...);
```

**优点**：不改框架
**缺点**：用户代码要改

---

## 推荐选择

| 场景 | 推荐方案 |
|-----|---------|
| 快速验证劫持 | 方案一（改返回类型） |
| 框架级支持，用户透明 | 方案二（手动转发）或 方案 A（CRTP + INode） |
| 不想改框架，只改自己代码 | 方案三（改用户代码） |
| 长期维护，需要劫持很多方法 | 方案 A（CRTP + INode） |

---

## 参考资料

- [C++ CRTP 模式详解](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
- [虚函数 vs 模板方法模式](https://refactoring.guru/design-patterns/template-method)
