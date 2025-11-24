#!/bin/sh

set -e

export DISABLE_MARCH_NATIVE=1

cmake --preset cross-arm64
cmake --build --preset cross-arm64-release
llvm-strip build/arm64/decoder