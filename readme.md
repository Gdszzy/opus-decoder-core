## opus-decoder-go

本项目旨在直接调用 libopus 完成 opus 解码，并使用本地编译参数获得最大执行性能

克隆仓库后先拉取 libopus

```shell
git submodule update --init --recursive
```

编译

```shell
./make-x64.sh
```

执行解码

```shell
./build/decoder src.opus output.wav
```

建议在 tmpfs 中执行编解码以便节省 IO 开销
