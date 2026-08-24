# local_any_demo

这个示例演示 Rust 程序如何通过 FFI 调用 RLM 的 C client library，并使用一个本地 `hostid=ANY`、`uncounted` 的 license 文件完成单机授权。

运行行为：

- 启动后默认读取当前项目目录下的 `demo.lic`
- 支持通过命令行参数指定别的 `lic` 文件
- checkout 产品 `test1`，版本 `1.0`，数量 `1`
- 成功后输出 `hello world`
- 进程保持运行，直到用户按 `Ctrl+C`
- 收到 `Ctrl+C` 后自动 `checkin` 并退出

## 前提

- 已有可用的 RLM SDK：默认使用 `/work-dir/dayu/rlm/rlm_kit`
- 本目录的 `demo.lic` 保持不变，它使用了 SDK 自带 demo key 的已签名 `test1` license

## 构建

```bash
cargo build
```

如果你的 SDK 路径不是默认值，可以覆盖环境变量：

```bash
RLM_SDK_DIR=/path/to/rlm.v17.0BL1-arm64_m2 cargo build
```

发布构建建议使用 glibc 目标：

```bash
cargo build --target x86_64-unknown-linux-gnu --release
```

`x86_64-unknown-linux-musl` 默认不受支持，因为当前仓库中的 `rlm_kit/x64_l1` 只包含 glibc 版 `rlm.a`/`libcrypto.a`。如果你手头有 musl 兼容的 OpenSSL 静态库，可以额外设置：

```bash
RLM_OPENSSL_LIB_DIR=/path/to/musl/openssl/lib cargo build --target x86_64-unknown-linux-musl --release
```

即便如此，仍然要求你的 RLM SDK 库本身兼容 musl；当前示例仓库未附带这类库。

## 运行

```bash
cargo run
```

默认会读取：

```text
<project>/demo.lic
```

如果你想直接指定别的本地 license 文件，运行：

```bash
cargo run -- --lic /absolute/path/to/your.lic
```

也支持短参数：

```bash
cargo run -- -l /absolute/path/to/your.lic
```

如果你更想通过环境变量指定，也可以设置：

```bash
RLM_LICENSE_SPEC=/absolute/path/to/your.lic cargo run
```

优先级如下：

```text
--lic / -l > RLM_LICENSE_SPEC > <project>/demo.lic
```

## 示例 license 类型

当前 `demo.lic` 中的 license 为：

- 产品：`test1`
- 版本：`1.0`
- 类型：`uncounted`
- hostid：`ANY`

这意味着它是一个本地可运行的 demo license，不依赖 license server，也不绑定具体机器。
