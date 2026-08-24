# float_demo

这个示例演示如何在 Rust 程序中通过 FFI 调用 RLM 的 C client library，并申请一个浮动许可。

运行行为：

- 启动后默认尝试连接 `5053@localhost`
- checkout 产品 `float_demo`，版本 `1.0`，数量 `1`
- 成功后输出 `hello world`
- 进程保持运行，直到用户按 `Ctrl+C`
- 收到 `Ctrl+C` 后自动 `checkin` 并退出

## 前提

- 已有可用的 RLM SDK：当前项目内为 `../../rlm_kit`
- 如未把 SDK 放到默认路径，请构建时设置 `RLM_SDK_DIR=/work-dir/dayu/rlm/rlm-Research/rlm_kit`
- 已在 `../../rlm_kit/x64_l1` 目录启动 `rlm`
- server 端有 `float_demo` 这条浮动许可，且并发数为 `2`

## 生成浮动 license

浮动许可需要 license server。license 文件中数量由 `LICENSE` 行里的 `count` 字段控制：

```text
LICENSE demo float_demo 1.0 permanent 2
```

字段含义：

- `demo`：ISV 名称，对应当前 SDK demo key。
- `float_demo`：产品名，必须和源码中的 checkout 产品名一致。
- `1.0`：版本，必须和源码中的 checkout 版本一致。
- `permanent`：永久有效，也可以换成具体过期日期。
- `2`：浮动许可总数量，即最多 2 个客户端同时 checkout。

如果要改并发数，就改最后这个数字。例如设置 10 个并发：

```text
LICENSE demo float_demo 1.0 permanent 10
```

生成步骤：

1. 查询 license server 机器的 hostid。

```bash
cd ../../rlm_kit/x64_l1
./rlmutil rlmhostid -q ether
```

2. 创建未签名浮动 license 文件。下面示例使用端口 `5053`、主机名 `localhost`、并发数 `2`，请把 `525400778466` 替换成 license server 机器的 hostid。

```bash
cat > float.lic <<'EOF'
HOST localhost 525400778466 5053
ISV demo
LICENSE demo float_demo 1.0 permanent 2
EOF
```

3. 使用 SDK 的 `rlmsign` 签名 license。

```bash
./rlmsign float.lic
```

签名后文件中会增加 `_ck` 和 `sig`。不要手工修改已签名内容；如果修改并发数、产品名、版本、hostid 或有效期，需要重新签名。

4. 使用该 license 启动 license server。

```bash
./rlm -c float.lic
```

启动后客户端默认连接 `5053@localhost`。如果 server 不在本机，运行 demo 时通过 `RLM_LICENSE_SPEC` 或命令行参数指定 `5053@your-server-host`。

## 构建

```bash
cargo build
```

如果你的 SDK 路径不是默认值，可以覆盖环境变量：

```bash
RLM_SDK_DIR=/work-dir/dayu/rlm/rlm-Research/rlm_kit cargo build
```

## 运行

```bash
cargo run
```

默认会连接：

```text
5053@localhost
```

如需改成别的 license server，可设置环境变量：

```bash
RLM_LICENSE_SPEC=5053@your-server-host cargo run
```

也可以直接通过命令行参数指定，优先级高于环境变量：

```bash
cargo run -- 5053@your-server-host
```

或：

```bash
cargo run -- --license 5053@your-server-host
```

## 验证浮动许可数量为 2

在 3 个终端里分别运行：

```bash
cargo run
```

预期结果：

- 第 1 个实例成功并输出 `hello world`
- 第 2 个实例成功并输出 `hello world`
- 第 3 个实例因 `float_demo` 无可用席位而失败
