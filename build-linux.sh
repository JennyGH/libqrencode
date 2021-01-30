#!/bin/bash
if [ ! -d build-linux ]; then
    mkdir build-linux
fi
cd build-linux
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release