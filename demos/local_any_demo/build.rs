use std::env;
use std::fs;
use std::path::{Path, PathBuf};

const DEFAULT_RLM_SDK_DIR: &str = "/work-dir/dayu/rlm/rlm_kit";

fn main() {
    let sdk_dir = env::var("RLM_SDK_DIR").unwrap_or_else(|_| DEFAULT_RLM_SDK_DIR.to_string());
    let platform_dir = Path::new(&sdk_dir).join("x64_l1");
    let rlm_a = platform_dir.join("rlm.a");
    let crypto_a = platform_dir.join("libcrypto.a");
    let target_os = env::var("CARGO_CFG_TARGET_OS").expect("CARGO_CFG_TARGET_OS is not set");

    println!("cargo:rerun-if-env-changed=RLM_SDK_DIR");
    println!("cargo:rerun-if-changed={}", rlm_a.display());
    println!("cargo:rerun-if-changed={}", crypto_a.display());

    if !rlm_a.exists() {
        panic!("RLM static library not found: {}", rlm_a.display());
    }
    if !crypto_a.exists() {
        panic!("RLM crypto library not found: {}", crypto_a.display());
    }

    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR is not set"));
    let vendored_rlm = out_dir.join("librlm.a");
    fs::copy(&rlm_a, &vendored_rlm).expect("failed to copy rlm.a into OUT_DIR");

    println!("cargo:rustc-link-search=native={}", out_dir.display());
    println!("cargo:rustc-link-search=native={}", platform_dir.display());
    println!("cargo:rustc-link-lib=static=rlm");
    println!("cargo:rustc-link-lib=static=crypto");

    match target_os.as_str() {
        "macos" => {
            println!("cargo:rustc-link-lib=objc");
            println!("cargo:rustc-link-lib=framework=CoreWLAN");
            println!("cargo:rustc-link-arg=-fobjc-arc");
            println!("cargo:rustc-link-arg=-mmacosx-version-min=12.0");
        }
        "linux" => {
            println!("cargo:rustc-link-lib=pthread");
            println!("cargo:rustc-link-lib=dl");
            println!("cargo:rustc-link-lib=rt");
        }
        _ => {}
    }
}
