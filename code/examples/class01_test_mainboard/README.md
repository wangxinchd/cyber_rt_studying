# 用于测试这个问题：

如果有两个节点planning.so 和 loc.so 这两个节点分别通过 mainborad 在同一个机器上面拉起来，是两个进程，按照cyber rt 规定采用共享内存通讯。
如果通过 mainboard 一次性拉起 planning.so  和 loc.so，此时是 一个进程中，是采用共享内存 还是指针copy呢




# 编译命令 (CMake)


# 运行命令
## 方式1: 同一个进程中 mainboard 拉起两个节点
./install/examples/class01_test_mainboard/scripts/start_reader_writer.sh 

## 方式2: 分别在两个进程中运行 (跨进程共享内存通讯)
在两个终端中分别启动下面两个脚本
./install/examples/class01_test_mainboard/scripts/start_reader_component.sh
./install/examples/class01_test_mainboard/scripts/start_writer_component.sh

# 需要创建两个 dag 文件:
#   - planning.dag (只包含 WriterComponent)
#   - loc.dag (只包含 ReaderComponent)

# 测试方法:
# 终端1: 运行 mainboard
# 终端2: 查看共享内存状态
#   - 如果使用 XSI IPC: ipcs -m

# 查看进程:
# ps aux | grep mainboard
