# Cyber RT 调度关系说明：TimerComponent 与 Component 的区别

## 1. 概述

Apollo Cyber RT 提供了两种主要的组件类型来处理数据：

| 特性 | TimerComponent | Component |
|------|---------------|-----------|
| **触发方式** | 定时触发（基于时间） | 数据驱动（消息到达） |
| **Init() 调用** | 初始化时调用一次 | 初始化时调用一次 |
| **Proc() 签名** | `bool Proc()` 无参数 | `bool Proc(const std::shared_ptr<T>& msg)` 带消息参数 |
| **订阅通道** | 无 | 支持 0-4 个通道 |
| **调度方式** | Timer 线程独立执行 | Cyber 调度器 + 协程 |

## 2. Init() 生命周期

**两者完全相同**：`Init()` 在组件初始化时被调用一次，必须返回 `true` 才表示初始化成功。

```cpp
// TimerComponent
bool TimerComponent::Initialize(const TimerComponentConfig& config) {
    // ...
    if (!Init()) {
        return false;  // Init() 失败则组件初始化失败
    }
    timer_->Start();  // Init() 成功后才启动定时器
    return true;
}

// Component
bool Component<M0>::Initialize(const ComponentConfig& config) {
    // ...
    if (!Init()) {
        return false;  // Init() 失败则组件初始化失败
    }
    // 创建读者、注册任务...
    return true;
}
```

**使用场景**：
- 创建 Writer/Reader
- 初始化计数器或状态
- 加载配置参数

## 3. Proc() 触发机制对比

### 3.1 TimerComponent - 时间驱动

```
┌─────────────────────────────────────────────────────────────┐
│                      TimerComponent                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌──────────────┐    固定间隔     ┌──────────────────┐    │
│   │  TimingWheel │ ──────────────► │ Proc() 被调用     │    │
│   │   (定时器)    │    (10ms 等)    │ (在 Timer 线程)   │    │
│   └──────────────┘                 └──────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**特点**：
- `Proc()` 在 Timer 线程中执行，**不参与 Cyber 调度器**
- 以固定时间间隔周期性执行
- `Proc()` 没有参数，不能直接访问输入数据
- 适合：传感器数据发布、心跳检测、定时任务

**TimerComponent::Proc() 示例**：

```cpp
bool TimerComponentSample::Init() {
    driver_writer_ = node_->CreateWriter<Driver>("/carstatus/channel");
    return true;
}

bool TimerComponentSample::Proc() {  // 每隔 interval 时间被调用一次
    static int i = 0;
    auto out_msg = std::make_shared<Driver>();
    out_msg->set_msg_id(i++);
    driver_writer_->Write(out_msg);  // 主动发布数据
    return true;
}
```

### 3.2 Component - 数据驱动

```
┌─────────────────────────────────────────────────────────────┐
│                        Component                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌──────────────┐                 ┌──────────────────┐    │
│   │   Reader     │ ── 消息到达 ──► │ Proc(msg) 被调用  │    │
│   │   (订阅者)   │                 │ (在 Scheduler 线程)│    │
│   └──────────────┘                 └──────────────────┘    │
│           │                                  │               │
│           ▼                                  ▼               │
│   ┌──────────────────────────────────────────────────────┐  │
│   │              Cyber Scheduler + Coroutine              │  │
│   │              (协程池管理任务执行)                       │  │
│   └──────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**特点**：
- `Proc()` 在 Cyber 调度器的协程线程中执行
- 当订阅的通道有新消息时触发
- 支持多通道数据同步（DataVisitor）
- 适合：数据处理流水线、感知融合、规划控制

**Component::Proc() 示例（单输入）**：

```cpp
bool PlanningComponent::Init() {
    localization_reader_ = node_->CreateReader<SimpleLocalization>(
        "channel/localization",
        [this](const std::shared_ptr<SimpleLocalization>& msg) {
            OnLocalization(msg);
        });
    planning_writer_ = node_->CreateWriter<SimplePlanning>("channel/planning_traj");
    return true;
}

void PlanningComponent::OnLocalization(const std::shared_ptr<SimpleLocalization>& msg) {
    // 处理收到的定位数据
    auto plan_msg = std::make_shared<SimplePlanning>();
    plan_msg->set_x(msg->x() + 10.0);
    plan_msg->set_y(msg->y() + 10.0);
    planning_writer_->Write(plan_msg);
}
```

**Component::Proc() 示例（定时器组件模式）**：

```cpp
class PlanningComponent : public Component<SimpleLocalization> {
 public:
    bool Init() override;
    bool Proc(const std::shared_ptr<SimpleLocalization>& msg) override;  // 接收消息
};
```

```cpp
bool PlanningComponent::Proc(const std::shared_ptr<SimpleLocalization>& msg) {
    // 收到定位数据时自动触发
    auto plan_msg = std::make_shared<SimplePlanning>();
    plan_msg->set_x(msg->x() + 10.0);
    planning_writer_->Write(plan_msg);
    return true;
}
```

## 4. DAG 配置对比

### 4.1 TimerComponent DAG 配置

```proto
module_config {
    module_library : "/apollo/lib/libcomponent.so"
    timer_components {
        class_name : "LocComponent"        // 组件类名
        config {
            name : "localization"           // 组件实例名
            interval : 100                  // 触发间隔（毫秒）
        }
    }
}
```

### 4.2 Component DAG 配置（单输入）

```proto
module_config {
    module_library : "./lib/libplanning.so"
    components {
        class_name : "PlanningComponent"   // 组件类名
        config {
            name : "planning"               // 组件实例名
            readers {
                channel : "channel/localization"  // 订阅的通道
            }
        }
    }
}
```

### 4.3 Component DAG 配置（多输入同步）

```proto
module_config {
    module_library : "./lib/libfusion.so"
    components {
        class_name : "FusionComponent"     // 组件类名
        config {
            name : "fusion"                // 组件实例名
            readers {
                channel : "/apollo/sensor/lidar"
            }
            readers {
                channel : "/apollo/sensor/radar"
            }
            readers {
                channel : "/apollo/sensor/camera"
            }
        }
    }
}
```

**多输入同步说明**：当所有订阅通道都有新数据时，`Proc()` 才会被调用（DataVisitor 机制）。

## 5. 代码结构对比

### 5.1 TimerComponent 完整示例

```cpp
// loc_component.h
#include "cyber/component/timer_component.h"

class LocComponent : public TimerComponent {
 public:
    bool Init() override;
    bool Proc() override;  // 无参数

 private:
    std::shared_ptr<Writer<SimpleLocalization>> localization_writer_;
    uint64_t seq_ = 0;
};
CYBER_REGISTER_COMPONENT(LocComponent)

// loc_component.cc
bool LocComponent::Init() {
    localization_writer_ = node_->CreateWriter<SimpleLocalization>("channel/localization");
    return true;
}

bool LocComponent::Proc() {  // 每隔配置的时间间隔执行一次
    auto loc_msg = std::make_shared<SimpleLocalization>();
    loc_msg->set_timestamp(Time::Now().ToNanosecond());
    loc_msg->set_seq(++seq_);
    loc_msg->set_x(100.0 + seq_ * 0.1);
    loc_msg->set_y(200.0 + seq_ * 0.1);
    localization_writer_->Write(loc_msg);
    return true;
}
```

### 5.2 Component 完整示例

```cpp
// planning_component.h
#include "cyber/component/component.h"
#include "examples/proto/simple_planning.pb.h"

class PlanningComponent : public Component<SimpleLocalization> {
 public:
    bool Init() override;
    bool Proc(const std::shared_ptr<SimpleLocalization>& msg) override;  // 带消息参数

 private:
    std::shared_ptr<Writer<SimplePlanning>> planning_writer_;
    uint64_t recv_count_ = 0;
};
CYBER_REGISTER_COMPONENT(PlanningComponent)

// planning_component.cc
bool PlanningComponent::Init() {
    planning_writer_ = node_->CreateWriter<SimplePlanning>("channel/planning_traj");
    return true;
}

bool PlanningComponent::Proc(const std::shared_ptr<SimpleLocalization>& msg) {
    recv_count_++;
    AINFO << "Received localization: seq=" << msg->seq();

    auto plan_msg = std::make_shared<SimplePlanning>();
    plan_msg->set_timestamp(Time::Now().ToNanosecond());
    plan_msg->set_seq(msg->seq());
    plan_msg->set_x(msg->x() + 10.0);
    plan_msg->set_y(msg->y() + 10.0);
    planning_writer_->Write(plan_msg);
    return true;
}
```

