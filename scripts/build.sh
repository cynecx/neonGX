#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

cd ..

rm -Rf ./build_web
mkdir ./build_web
cd ./build_web
cmake -DCMAKE_BUILD_TYPE=Release -DNEONGX_BUILD_USING_EMSCRIPTEN=1 ..
make -j8
cd ..

rm -Rf ./build_linux
mkdir ./build_linux
cd ./build_linux
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
cd ..
