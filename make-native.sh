#!/bin/sh

set -e

cmake --preset native-release
cmake --build build/release
llvm-strip build/release/decoder