#!/bin/bash

curr_path=`pwd`

third_party="${curr_path}/code/third_party"
CMAKE_INSTALL_PREFIX_path="${curr_path}/code/env"

BUILD_TYPE="Debug"

if [ $# -ge 1 ]; then
    case "$1" in
        debug|Debug)
            BUILD_TYPE="Debug"
            ;;
        release|Release)
            BUILD_TYPE="Release"
            ;;
        *)
            echo "Usage: $0 [debug|release]"
            echo "  debug   - Debug mode (default)"
            echo "  release - Release mode"
            exit 1
            ;;
    esac
fi

cd "${curr_path}/code/env"
source "${curr_path}/code/setup.bash"

echo " ========== start printf env ========== "
echo "PATH: $PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "CMAKE_PREFIX_PATH: $CMAKE_PREFIX_PATH"
echo "PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
echo "BUILD_TYPE: $BUILD_TYPE"
echo " ========== end printf env ========== "


cd "${curr_path}/code"
env/bin/protoc -I=cyber/proto/ --cpp_out=cyber/proto cyber/proto/*.proto
env/bin/protoc -I=cyber/examples/proto/ --cpp_out=cyber/examples/proto cyber/examples/proto/*.proto
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build

if [ "$BUILD_TYPE" = "Debug" ]; then
    cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_PREFIX="${curr_path}/install" \
      -DCMAKE_CXX_FLAGS_DEBUG="-g -O0 -fno-omit-frame-pointer" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      ..
else
    cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="${curr_path}/install" \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      ..
fi

cmake --build . --target install -j$(nproc)
