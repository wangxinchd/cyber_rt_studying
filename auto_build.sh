# docker build -f ./DockerFile -t cyber_rt_build:0.1 .

curr_path=`pwd`

third_party="${curr_path}/code/third_party"
CMAKE_INSTALL_PREFIX_path="${curr_path}/code/env"

echo "third_party_path: ${third_party}"
echo "CMAKE_INSTALL_PREFIX_path: ${CMAKE_INSTALL_PREFIX_path}"

cd $third_party
tar -xvf tinyxml2-6.0.0.tar.gz
cd tinyxml2-6.0.0/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install


cd $third_party
tar -xvf gflags-2.2.2.tar.gz
cd gflags-2.2.2/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install


cd $third_party
tar -xvf googletest-release-1.12.1.tar.gz
cd googletest-release-1.12.1/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install -j$(nproc)

cd $third_party
tar -xvf glog-0.6.0.tar.gz
cd glog-0.6.0/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install -j$(nproc)

cd $third_party
tar -xvf protobuf-3.14.0.tar.gz
cd protobuf-3.14.0/
./autogen.sh
./configure --prefix=${CMAKE_INSTALL_PREFIX_path} CFLAGS="-fPIC" CXXFLAGS="-fPIC"
make -j$(nproc)
make install -j$(nproc)

cd $third_party
tar -xvf memory-0.7-3.tar.gz
cd memory-0.7-3/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DBUILD_SHARED_LIBS=ON ..
cmake --build . --target install -j$(nproc)


cd $third_party
tar -xvf Fast-CDR-1.0.24.tar.gz
cd Fast-CDR-1.0.24/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} ..
cmake --build . --target install -j$(nproc)


cd $third_party
tar -xvf Fast-DDS-2.7.1.tar.gz
cd Fast-DDS-2.7.1/
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX_path} -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX_path} ..
cmake --build . --target install -j$(nproc)


cd "${curr_path}/code/env"
source setup.bash
cd "${curr_path}/code"
env/bin/protoc -I=cyber/proto/ --cpp_out=cyber/proto cyber/proto/*.proto
env/bin/protoc -I=cyber/examples/proto/ --cpp_out=cyber/examples/proto cyber/examples/proto/*.proto
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX="${curr_path}/install" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --target install -j$(nproc)
