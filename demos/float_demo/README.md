# float_demo

这个示例演示如何在 Rust 程序中通过 FFI 调用 RLM 的 C client library，并申请一个浮动许可。

运行行为：

- 启动后默认尝试连接 `5053@localhost`
- checkout 产品 `float_demo`，版本 `1.0`，数量 `1`
- 成功后输出 `hello world`
- 进程保持运行，直到用户按 `Ctrl+C`
- 收到 `Ctrl+C` 后自动 `checkin` 并退出

## 前提

- 已有可用的 RLM SDK：`/Users/zwk/src_code/wp-labs/wp-ai/rlm.v17.0BL1-arm64_m2`
- 已在 SDK 的 `arm64_m2` 目录启动 `rlm`
- server 端有 `float_demo` 这条浮动许可，且并发数为 `2`

## 构建

```bash
cargo build
```

如果你的 SDK 路径不是默认值，可以覆盖环境变量：

```bash
RLM_SDK_DIR=/path/to/rlm.v17.0BL1-arm64_m2 cargo build
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

