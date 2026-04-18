# Class02: 测试 Callback 机制

本示例演示 Apollo Cyber RT 中 Reader/Writer 与 Callback 的使用：

- **WriterComponent**: 定时发送 Planning 消息，接收 Localization 回调
- **ReaderComponent**: 接收 Planning 消息，发送 Localization 消息

## 编译

```bash
# 在项目根目录
mkdir build && cd build
cmake ..
make
```

## 运行

### 方式1: 同一个进程中运行
```bash
./install/bin/mainboard \
    -d ./install/examples/class02_test_callback/dag/class02_writer.dag \
    -d ./install/examples/class02_test_callback/dag/class02_reader.dag
```

### 方式2: 分别在两个进程中运行
终端1:
```bash
./install/bin/mainboard -d ./install/examples/class02_test_callback/dag/class02_writer.dag
```

终端2:
```bash
./install/bin/mainboard -d ./install/examples/class02_test_callback/dag/class02_reader.dag
```

## 数据流

```
WriterComponent                    ReaderComponent
    |                                   |
    | -------- planning channel ------> |
    |                                   |
    | <-------- localization channel --- |
    |                                   |
```

## 查看通道
```bash
cyber_channel list | grep channel
```
