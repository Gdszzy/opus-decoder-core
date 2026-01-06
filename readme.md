## opus-decoder-core

本项目封装了 libopus 调用来实现解码工牌录音并输出wav

- ☑️ 一键编译脚本
- ☑️ 支持本地CPU编译参数，完全释放设备性能
- ☑️ 支持标准输入输出流，可实现流式解码或结合ffmpeg实现多核编解码
- ☑️ 支持Windows/Linux

### 用法

下载[预编译版本]()，或参考[编译](#编译)部分自行编译

建议生产环境中，使用在生产环境本地编译的版本，以便提高解码速度


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

使用标准输出流，结合ffmpeg开启双核编解码

```bash
./decoder -i input.opus -os | ffmpeg -i - output.mp3
```

使用标准输入流，实现流式解码

```bash
cat input.opus | ./decoder --is $(stat -c %s input.opus) -o output.wav
```

### 编译

需要先安装以下依赖

- cmake
- clang
- git
- ninja
- 如果需要交叉编译，自行安装交叉编译工具链

#### 本地编译

本地编译默认会启用`-march=native`，编译后无法移植到其他设备使用，但可最大化利用设备性能

```shell
./make-native.sh
```

如果需要可移植版本，可以使用参数禁用`-march=native`

```shell
DISABLE_MARCH_NATIVE=1 ./make-native.sh
```

#### 交叉编译

目前支持交叉编译到 arm64，但推荐在目标平台执行本地编译可获得更高的性能

```shell
./make-cross-arm64.sh
```

#### Windows平台

```powershell
# 允许执行ps1
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
# 执行编译
.\make-native.ps1
```