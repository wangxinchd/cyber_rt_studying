TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#################################################################
# 新环境系统还需要安装如下的依赖
# sudo apt-get install build-essential -y
# sudo apt install libasio-dev openssl libssl-dev -y
# sudo apt install libp11-dev libengine-pkcs11 softhsm2 libpoco-dev uuid-dev libncurses5-dev autoconf automake libtool -y
# 安装tinyxml2、protobuf、gflags、gtest、glog、foonathan_memory、fast-cdr、fast-dds 、cyber-rt
# 执行如下命令配置protoc环境，以及生成proto对应的pb.h和pb.cc文件
# 工程目录下启动Terminal，执行
# source protoc.sh
# ../../env/bin/protoc -I=src/ --cpp_out=src/ src/*.proto
#################################################################

LIBS += -L"$$PWD/../../../install/lib" -lcyber -lcommon_component_example
LIBS += -L"$$PWD/../../env/lib" -lfastcdr -lfastrtps -lfoonathan_memory-0.7.1 \
                      -lgflags -lgtest -lglog -ltinyxml2 -lprotobuf -lprotobuf-lite -lprotoc
LIBS += -lpthread

DESTDIR = $$PWD/bin

INCLUDEPATH += \
    $$PWD/../../../install/include \
    $$PWD/../../env/include \
    $$PWD/src \
    $$PWD/inc

HEADERS += \
    src/bussiness_data_info.pb.h \
    src/bussiness_global.h \
    src/bussiness_reader.h \
    src/bussiness_writer.h

SOURCES += \
    src/bussiness_data_info.pb.cc \
    src/bussiness_global.cpp \
    src/bussiness_reader.cxx \
    src/bussiness_writer.cxx \
    src/main.cpp




