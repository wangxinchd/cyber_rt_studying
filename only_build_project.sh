#!/bin/bash

curr_path=`pwd`

third_party="${curr_path}/code/third_party"
CMAKE_INSTALL_PREFIX_path="${curr_path}/code/env"

cd "${curr_path}/code/env"
source setup.bash

echo " ========== start printf env ========== "
echo "PATH: $PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "CMAKE_PREFIX_PATH: $CMAKE_PREFIX_PATH"
echo "PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
echo " ========== end printf env ========== "


cd "${curr_path}/code"
env/bin/protoc -I=cyber/proto/ --cpp_out=cyber/proto cyber/proto/*.proto
env/bin/protoc -I=cyber/examples/proto/ --cpp_out=cyber/examples/proto cyber/examples/proto/*.proto
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX="${curr_path}/install" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --target install -j$(nproc)
