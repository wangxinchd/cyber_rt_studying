# Class03: 不同类型 Component 通信

本示例演示 Apollo Cyber RT 中两种不同类型 Component 的使用：

- **LocComponent**: TimerComponent（时间触发），定时发送 Localization 消息
- **PlanningComponent**: Component（数据触发），接收 Localization 并发送 Planning 消息

## 编译

```bash
# 在项目根目录
mkdir build && cd build
cmake ..
make
```

## 运行

### 方式1: 同时运行 Loc 和 Planning
```bash
./install/examples/class03_different_component/class01_empty_component/scripts/start_class03_planning_loc.sh
```

### 方式2: 分别运行单个组件
终端1 (LocComponent):
```bash
./install/examples/class03_different_component/class01_empty_component/scripts/start_class03_loc_component.sh
```

终端2 (PlanningComponent):
```bash
./install/examples/class03_different_component/class01_empty_component/scripts/start_class03_planning_component.sh
```

## 数据流

```
LocComponent                    PlanningComponent
(TimerComponent)                   (Component)
    |                                   |
    | <------- planning channel -------|  (可选接收)
    |                                   |
    | ----> localization channel -----> |  (触发回调)
    |                                   |
```

## TimerComponent vs Component

| 特性 | TimerComponent | Component |
|------|---------------|-----------|
| 触发方式 | 定时触发（时间驱动） | 数据触发（回调驱动） |
| Proc 函数 | 必须实现 `bool Proc()` | 无 |
| Init 函数 | 必须实现 `bool Init()` | 必须实现 `bool Init()` |
| DAG 配置 | 使用 `timer_components` | 使用 `components` |

## DAG 配置示例

TimerComponent (`dag/class03_loc.dag`):
```proto
module_config {
    module_library : "./lib/libtest_class03_loc.so"
    timer_components {
        class_name : "LocComponent"
        config {
            name : "loc"
            interval : 500  # 500ms
        }
    }
}
```

Component (`dag/class03_planning.dag`):
```proto
module_config {
    module_library : "./lib/libtest_class03_planning.so"
    components {
        class_name : "PlanningComponent"
        config {
            name : "planning"
            readers {
                channel : "channel/localization"
            }
        }
    }
}
```

## 查看通道
```bash
cyber_channel list | grep channel
```
