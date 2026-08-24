mod rlm;

use std::env;
use std::path::PathBuf;
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;
use std::time::Duration;

use rlm::LicenseManager;

static SHOULD_EXIT: AtomicBool = AtomicBool::new(false);

fn main() {
    if let Err(message) = run() {
        eprintln!("RLM local-any demo failed: {message}");
        std::process::exit(1);
    }
}

fn run() -> Result<(), String> {
    ctrlc::set_handler(|| {
        SHOULD_EXIT.store(true, Ordering::SeqCst);
    })
    .map_err(|err| format!("failed to install Ctrl+C handler: {err}"))?;
    println!("单机任意实例类型");
    let license_spec = resolve_license_spec()?;

    let app_path = current_executable_directory()?;
    let manager = LicenseManager::new(&license_spec, &app_path)?;
    let license = manager.checkout("local_any_demo", "1.0", 1)?;

    println!("运行成功：hello world");
    println!("已经获得许可 local-any license 'local_any_demo' using {license_spec}");
    println!("press Ctrl+C to release the license and exit");

    while !SHOULD_EXIT.load(Ordering::SeqCst) {
        thread::sleep(Duration::from_millis(250));
    }

    license.checkin();
    drop(manager);
    println!("license released");
    Ok(())
}

fn resolve_license_spec() -> Result<String, String> {
    let mut args = env::args().skip(1);
    let mut license_spec = None;

    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--lic" | "-l" => {
                let value = args
                    .next()
                    .ok_or_else(|| "missing value for --lic".to_string())?;
                license_spec = Some(value);
            }
            "--help" | "-h" => {
                print_usage();
                std::process::exit(0);
            }
            _ => {
                return Err(format!(
                    "unknown argument: {arg}\n\nusage: cargo run -- [--lic /path/to/license.lic]"
                ));
            }
        }
    }

    Ok(license_spec
        .or_else(|| env::var("RLM_LICENSE_SPEC").ok())
        .unwrap_or_else(default_license_spec))
}

fn default_license_spec() -> String {
    let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    path.push("demo.lic");
    path.to_string_lossy().into_owned()
}

fn print_usage() {
    println!("usage: cargo run -- [--lic /path/to/license.lic]");
    println!("fallback order: --lic > RLM_LICENSE_SPEC > ./demo.lic");
}

fn current_executable_directory() -> Result<String, String> {
    let exe =
        env::current_exe().map_err(|err| format!("failed to resolve current executable: {err}"))?;
    let parent: PathBuf = exe
        .parent()
        .ok_or_else(|| "current executable has no parent directory".to_string())?
        .to_path_buf();

    parent
        .into_os_string()
        .into_string()
        .map_err(|_| "executable directory is not valid UTF-8".to_string())
}
