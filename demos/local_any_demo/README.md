# local_any_demo

这个示例演示 Rust 程序如何通过 FFI 调用 RLM 的 C client library，并使用一个本地 `hostid=ANY`、`uncounted` 的 license 文件完成单机授权。

运行行为：

- 启动后默认读取当前项目目录下的 `demo.lic`
- 支持通过命令行参数指定别的 `lic` 文件
- checkout 产品 `local_any_demo`，版本 `1.0`，数量 `1`
- 成功后输出 `hello world`
- 进程保持运行，直到用户按 `Ctrl+C`
- 收到 `Ctrl+C` 后自动 `checkin` 并退出

## 前提

- 已有可用的 RLM SDK：当前项目内为 `../../rlm_kit`
- 如未把 SDK 放到默认路径，请构建时设置 `RLM_SDK_DIR=/work-dir/dayu/rlm/rlm-Research/rlm_kit`
- 运行前需要准备本地 license 文件，默认文件名为 `demo.lic`

## 生成本地 license

这个 demo 使用本地 `uncounted` license，不依赖 license server。license 的关键字段是：

```text
LICENSE demo local_any_demo 1.0 permanent uncounted hostid=ANY
```

字段含义：

- `demo`：ISV 名称，对应当前 SDK demo key。
- `local_any_demo`：产品名，必须和源码中的 checkout 产品名一致。
- `1.0`：版本，必须和源码中的 checkout 版本一致。
- `permanent`：永久有效，也可以换成具体过期日期。
- `uncounted`：不计数许可，适合本地单机授权。
- `hostid=ANY`：不绑定机器；如果要绑定机器，可以改成具体 hostid。

生成步骤：

1. 查询本机 hostid，可选。如果使用 `hostid=ANY`，这一步可以跳过。

```bash
../../rlm_kit/x64_l1/rlmutil rlmhostid -q ether
```

2. 在本 demo 目录创建未签名 license 文件。

```bash
cat > demo.lic <<'EOF'
LICENSE demo local_any_demo 1.0 permanent uncounted hostid=ANY
EOF
```

如果要绑定机器，把 `hostid=ANY` 替换为上一步得到的 hostid，例如：

```text
LICENSE demo local_any_demo 1.0 permanent uncounted hostid=525400778466
```

3. 使用 SDK 的 `rlmsign` 签名 license。

```bash
../../rlm_kit/x64_l1/rlmsign demo.lic
```

签名后文件中会增加 `_ck` 和 `sig`。不要手工修改已签名内容；如果修改产品名、版本、hostid 或有效期，需要重新签名。

## 构建

```bash
cargo build
```

如果你的 SDK 路径不是默认值，可以覆盖环境变量：

```bash
RLM_SDK_DIR=/work-dir/dayu/rlm/rlm-Research/rlm_kit cargo build
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

- 产品：`local_any_demo`
- 版本：`1.0`
- 类型：`uncounted`
- hostid：`ANY`

这意味着它是一个本地可运行的 demo license，不依赖 license server，也不绑定具体机器。
