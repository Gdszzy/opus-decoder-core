## opus-decoder-core

本项目旨在直接调用 libopus 完成 opus 解码，并使用本地编译参数获得最大执行性能

### 用法

```shell
Opus decoder cli


./decoder [OPTIONS]


OPTIONS:
  -h,     --help              Print this help message and exit
[Option Group: input]
  Input options
  [Exactly 1 of the following options are required]


OPTIONS:
  -i TEXT                     Input from file
          --is INT            Input from stdin. Need provide file length
[Option Group: output]
  Output options
  [Exactly 1 of the following options are required]


OPTIONS:
  -o TEXT                     Output to file
          --os                Output to stdout
```

直接解码 opus 流为 wav 文件，建议输出到 tmpfs 挂载的目录以便降低 IO 开销

```bash
./decoder -i input.opus -o output.wav
```

解码同时调用 ffmpeg 编码为指定格式，由于解码和 ffmpeg 编码两个进程并行运行，可以获得最大吞吐性能

```bash
./decoder -i input.opus -os | ffmpeg -i - output.mp3
```

### 依赖

- cmake
- clang
- git

### 本地编译

本地编译默认会启用-march=native，编译后无法移植到其他设备使用

```shell
./make-native.sh
```

如果需要临时给其他设备编译一下，可以临时禁用-march=native

```shell
DISABLE_MARCH_NATIVE=1 ./make-native.sh
```

### 交叉编译

目前支持交叉编译到 arm64，但推荐在目标平台执行本地编译可获得更高的性能

```shell
./make-cross-arm64.sh
```
