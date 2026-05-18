#!/bin/bash
# rebuild.sh

# Remove old build directory and make a fresh one
rm -rf build
mkdir build
cd build

# Configure with CMake in Debug mode
#cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build using all CPU cores
make -j$(nproc)

cd ..
