#!/bin/bash

curr_path=`pwd`

third_party="${curr_path}/code/third_party"
CMAKE_INSTALL_PREFIX_path="${curr_path}/code/env"

rm -rf ${curr_path}/code/cyber/proto/*.pb.*
rm -rf ${curr_path}/code/cyber/examples/proto/*.pb.*

rm -rf ${curr_path}/code/build
rm -rf ${curr_path}/code/third_party/gflags-2.2.2
rm -rf ${curr_path}/code/third_party/glog-0.6.0
rm -rf ${curr_path}/code/third_party/googletest-release-1.12.1
rm -rf ${curr_path}/code/third_party/memory-0.7-3
rm -rf ${curr_path}/code/third_party/protobuf-3.14.0
rm -rf ${curr_path}/code/third_party/tinyxml2-6.0.0
rm -rf ${curr_path}/code/third_party/Fast-CDR-1.0.24
rm -rf ${curr_path}/code/third_party/Fast-DDS-2.7.1
