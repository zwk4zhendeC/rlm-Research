# RLM 调研文档

本项目用于记录 RLM（Reprise License Manager）授权体系调研结果，并保留少量 Rust 调用 RLM C client library 的验证 demo。

## 关键文件与目录

| 路径 | 作用 |
| --- | --- |
| `调研.md` | 主调研文档，说明 RLM 解决的问题、常见许可类型、核心组件、关键术语，以及采购、交付、授权等基本流程。 |
| `assets/调研/` | `调研.md` 引用的图片资源，包含部署结构、流程图等说明图片。 |
| `uml/` | PlantUML 源文件，用于维护调研文档中的关键流程图。 |
| `uml/采购流程.puml` | 描述 ISV 采购 RLM、生成密钥、集成 SDK/公钥并构建交付物的流程。 |
| `uml/客户购买流程.puml` | 描述客户购买产品授权后，ISV 交付产品、许可服务器包和 license 文件的流程。 |
| `uml/客户内网授权流程.puml` | 描述客户内网中客户端连接 license server、checkout/checkin 许可的授权流程。 |
| `RLM_Reference.pdf` | RLM 参考资料，用于查询更完整的接口、概念和配置说明。 |
| `demos/` | RLM 集成验证示例目录。 |
| `demos/local_any_demo/` | Rust FFI 调用 RLM C client library 的本地 license 示例，默认读取本目录 `demo.lic`，演示不依赖 license server 的本地授权。 |
| `demos/float_demo/` | Rust FFI 调用 RLM C client library 的浮动许可示例，默认连接 `5053@localhost`，用于验证 license server 并发席位控制。 |
| `rlm_kit/` | 当前项目内的 RLM v17.0BL1 x64 Linux SDK 目录，包含头文件、静态库、命令行工具、license 文件和官方示例；demo 构建脚本默认从这里链接 `rlm.a`、`libcrypto.a`。 |
| `test/` | 另一份 RLM SDK/测试样例内容，用于对照原始 SDK 结构和官方 C 示例。 |

## Demo 目录说明

两个 demo 的结构基本一致：

| 路径 | 作用 |
| --- | --- |
| `Cargo.toml` / `Cargo.lock` | Rust 项目配置与依赖锁定文件。 |
| `build.rs` | 构建脚本，按 `RLM_SDK_DIR` 定位 RLM SDK，并链接 `rlm.a`、`libcrypto.a` 等静态库。 |
| `src/main.rs` | 示例程序入口，负责解析 license 配置、调用 checkout、等待 `Ctrl+C` 后 checkin。 |
| `src/rlm.rs` | 对 RLM C API 的最小 FFI 封装，包括 `rlm_init`、`rlm_checkout`、`rlm_checkin`、错误信息读取和资源释放。 |
| `README.md` | 对应 demo 的构建、运行和验证说明。 |

## RLM SDK 与示例

`rlm_kit/` 是当前 demo 主要依赖的 SDK 目录，版本为 RLM v17.0BL1。它适合用来查 RLM 原始 C API、链接库、工具和官方示例：

| 路径 | 作用 |
| --- | --- |
| `rlm_kit/src/VERSION` | SDK 版本信息，当前为 `RLM v17.0 (build 1, 17-apr-2025)`。 |
| `rlm_kit/src/RELEASE_NOTES` | 当前 SDK 版本的发布说明、平台支持、已知问题和 API 变化。 |
| `rlm_kit/src/license.h` | RLM client/ISV 相关 C API 的主要头文件，Rust FFI 封装可对照这里确认函数签名和常量。 |
| `rlm_kit/src/rlm_admin.h` | RLM 管理相关接口头文件。 |
| `rlm_kit/src/rlm_isv_config.c` | ISV 侧配置相关源码，可用于理解 ISV server/customization 的配置入口。 |
| `rlm_kit/src/rlm_pubkey.c` / `rlm_kit/src/rlm_privkey.c` | ISV 公钥/私钥相关源码，配合 license 生成、签名和服务端校验流程理解。 |
| `rlm_kit/x64_l1/` | x64 Linux 平台的 SDK 库、工具、已编译示例和 license 文件，包括 `rlm`、`rlmutil`、`rlmgen`、`rlmgenkeys`、`rlmsign`、`rlm.a`、`lib_client.a`、`libcrypto.a`、`float.lic`、`example.lic` 等。 |
| `rlm_kit/examples/` | 官方 C 示例代码，覆盖基础 checkout、激活、rehost、roam、transfer 等场景。 |

`test/` 目录结构与 `rlm_kit/` 接近，也保留了 RLM SDK 的 `src/`、`x64_l1/` 和 `examples/`，可作为对照样例使用。它的 `test/examples/` 中包含同类官方 C 示例，适合在不改动 `rlm_kit/` 的情况下阅读或实验。

`rlm_kit/examples/` 与 `test/examples/` 中几个常用示例：

| 路径 | 作用 |
| --- | --- |
| `*/examples/sampleclient.c` | 基础客户端示例，演示 `rlm_init`、`rlm_checkout`、状态检查和 `rlm_checkin` 的典型调用流程。 |
| `*/examples/example2.c` | 演示自动 heartbeat、请求 option set，并 checkout `actdemo` 许可。 |
| `*/examples/act_api_example.c` | RLM internet activation 示例，演示未找到 license 时通过 activation 获取 license 后重新初始化。 |
| `*/examples/rehost_example.c` | license rehosting/revoking 示例，演示可换机授权相关流程。 |
| `*/examples/roam_example.c` | roaming license 示例，演示通过 `RLM_ROAM` 使用或归还可漫游许可。 |
| `*/examples/rlm_transfer.c` | license transfer 相关示例。 |
| `*/examples/detached_demo.c` | detached license 相关示例。 |
| `*/examples/unsupported/` | 官方标记为 unsupported 的补充示例，包括 Python 和 Fortran 集成示例，仅适合作参考。 |

## 建议阅读顺序

1. 先读 `调研.md`，理解 RLM 的核心概念和授权流程。
2. 再看 `uml/` 下的 PlantUML 文件，确认流程图源文件如何维护。
3. 对照 `rlm_kit/src/license.h`、`rlm_kit/examples/` 和 `test/examples/`，确认 RLM C API 的原始用法。
4. 如需验证 Rust 集成方式，再进入 `demos/local_any_demo/` 或 `demos/float_demo/` 查看示例说明与源码。
5. 遇到接口或配置细节时，查阅 `RLM_Reference.pdf`。
