#!/bin/sh

set -e

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j8
./decoder /home/ghost/文档/xwechat_files/wxid_s88qoerexckx22_851a/msg/file/2025-03/test_2.0.0_audio.data-s0 out.wav