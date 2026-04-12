#!/bin/bash

curr_path=`pwd`

third_party="${curr_path}/code/third_party"
CMAKE_INSTALL_PREFIX_path="${curr_path}/code/env"


rm -rf ${curr_path}/code/cyber/proto/*.pb.*
rm -rf ${curr_path}/code/cyber/examples/proto/*.pb.*

rm -rf ${curr_path}/code/build