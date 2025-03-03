#!/bin/sh

set -e

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j8
./testMain