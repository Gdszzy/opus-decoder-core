#!/bin/sh

set -e

cmake --preset native
cmake --build --preset native-release
llvm-strip build/decoder