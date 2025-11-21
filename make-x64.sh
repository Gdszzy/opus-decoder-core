#!/bin/sh

set -e

cmake --preset default
cmake --build --preset release
strip build/decoder