#!/bin/sh

set -e

cmake --preset cross-arm64
cmake --build build/arm64
llvm-strip build/arm64/decoder