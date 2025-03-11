#!/bin/sh

set -e

export CC=aarch64-linux-gnu-gcc
export CXX=aarch64-linux-gnu-g++

cmake -B build-aarch64
cmake --build build-aarch64 --parallel 8