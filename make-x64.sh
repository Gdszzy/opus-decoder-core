#!/bin/sh

set -e

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 8
strip build/decoder