# Apollo Cyber RT Component 模板特化详解

## 概述

`Component` 是 Cyber RT 框架中的核心组件类，支持处理最多 4 个通道的消息。通过 C++ 模板特化机制，框架为不同数量的消息类型提供了专门的处理逻辑。

## 模板层次结构

```
Component<M0, M1, m2, M3>  (主模板 - 通用基类)
    │
    ├── Component<NullType, NullType, NullType, NullType>  (无消息)
    ├── Component<M0, NullType, NullType, NullType>        (1个消息)
    ├── Component<M0, M1, NullType, NullType>             (2个消息)
    ├── Component<M0, M1, M2, NullType>                    (3个消息)
    └── Component<M0, M1, M2, M3>                         (4个消息)
```

## 1. 主模板 (Primary Template) - 第57-91行

```cpp
template <typename M0 = NullType, typename M1 = NullType,
          typename M2 = NullType, typename M3 = NullType>
class Component : public ComponentBase {
  // 有纯虚函数 Proc()，需要派生类实现
  virtual bool Proc(...) = 0;
};
```

特点：包含纯虚函数 `Proc()`，不能直接实例化。

## 2. 全特化版本

### 什么是模板特化？

C++ 模板有两种特化方式：

**偏特化 (Partial Specialization)**：部分固定模板参数
```cpp
template <typename M0>  // 只特化第一个参数，其余仍是模板参数
class Component<M0, NullType, NullType, NullType> { ... }
```

**全特化 (Full Specialization)**：所有模板参数都固定
```cpp
template <>  // 空模板参数列表，表示全部参数已固定
class Component<NullType, NullType, NullType, NullType> { ... }
```

### Component 的特化策略

| 特化版本 | 模板参数 | 特化类型 |
|---------|---------|---------|
| 第1个 | `<>` 全部为 NullType | 全特化 |
| 第2个 | `<M0, NullType, NullType, NullType>` | 偏特化 |
| 第3个 | `<M0, M1, NullType, NullType>` | 偏特化 |
| 第4个 | `<M0, M1, M2, NullType>` | 偏特化 |

### 为什么需要特化？

1. **避免纯虚函数**：主模板有 `Proc() = 0`，但全特化版本不需要消息处理
2. **优化代码**：不同消息数量有不同的初始化逻辑
3. **类型安全**：编译时确定类型，避免运行时判断

### 2.1 Component<> (无消息类型) - 第93-112行

```cpp
template <>
class Component<NullType, NullType, NullType, NullType> : public ComponentBase {
  // 无 Proc 纯虚函数，有完整的 Initialize 实现
};
```

**用途**：用于不需要订阅消息的组件，只需在 `Init()` 中执行初始化逻辑。

### 2.2 Component<M0> (1个消息) - 第114-124行

```cpp
template <typename M0>
class Component<M0, NullType, NullType, NullType> : public ComponentBase {
  virtual bool Proc(const std::shared_ptr<M0>& msg) = 0;
};
```

**用途**：订阅单个通道的消息。

### 2.3 Component<M0, M1> (2个消息) - 第126-138行

```cpp
template <typename M0, typename M1>
class Component<M0, M1, NullType, NullType> : public ComponentBase {
  virtual bool Proc(const std::shared_ptr<M0>& msg,
                    const std::shared_ptr<M1>& msg1) = 0;
};
```

**用途**：订阅两个通道的消息。

### 2.4 Component<M0, M1, M2> (3个消息) - 第140-153行

```cpp
template <typename M0, typename M1, typename M2>
class Component<M0, M1, M2, NullType> : public ComponentBase {
  virtual bool Proc(const std::shared_ptr<M0>& msg,
                    const std::shared_ptr<M1>& msg1,
                    const std::shared_ptr<M2>& msg2) = 0;
};
```

**用途**：订阅三个通道的消息。

## 3. Initialize 流程 (以1消息为例，第175-234行)

```
Initialize(config)
    │
    ├── 1. 创建 Node
    ├── 2. 加载配置文件
    ├── 3. 验证 readers 数量
    ├── 4. 调用用户 Init()
    │
    ├── 5. 创建 Reader
    │       │
    │       ├── 现实模式：reader = node_->CreateReader<M0>(config)
    │       └── 仿真模式：reader = node_->CreateReader<M0>(config, func)
    │
    └── 6. 创建调度任务
            │
            └── sched->CreateTask(factory, node_name)
```

## 4. 仿真模式 vs 现实模式

- **现实模式 (Reality Mode)**：`is_reality_mode = true`，使用真实数据源
- **仿真模式 (Simulation Mode)**：使用 Blocker 缓存数据，支持数据回放

## 5. 用户使用示例

```cpp
// 无消息组件
class MyComponent : public Component<> {
  bool Init() override { /* 初始化 */ return true; }
};

// 单消息组件
class SensorComponent : public Component<Image> {
  bool Proc(const std::shared_ptr<Image>& msg) override {
    // 处理图像
    return true;
  }
};

// 注册
CYBER_REGISTER_COMPONENT(MyComponent)
CYBER_REGISTER_COMPONENT(SensorComponent)
```

## 6. 关键设计点

1. **模板特化**：为不同消息数量提供专门实现，避免运行时类型判断
2. **纯虚函数**：用户必须实现 `Proc()` 定义消息处理逻辑
3. **Init() 钩子**：用户实现 `Init()` 进行初始化
4. **数据同步**：通过 `DataVisitor` 保证多消息的时间同步
5. **智能指针**：使用 `shared_from_this()` 和 `weak_ptr` 避免循环引用
