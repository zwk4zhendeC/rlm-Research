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
        eprintln!("RLM floating license demo failed: {message}");
        std::process::exit(1);
    }
}

fn run() -> Result<(), String> {
    ctrlc::set_handler(|| {
        SHOULD_EXIT.store(true, Ordering::SeqCst);
    })
    .map_err(|err| format!("failed to install Ctrl+C handler: {err}"))?;
    println!("我是浮动实例类型");
    let license_spec = resolve_license_spec(env::args().skip(1))?;
    let app_path = current_executable_directory()?;
    let manager = LicenseManager::new(&license_spec, &app_path)?;
    let license = manager.checkout("float_demo", "1.0", 1)?;

    println!("运行成功：hello world");
    println!("checked out floating license 'float_demo' via {license_spec}");
    println!("press Ctrl+C to release the license and exit");

    while !SHOULD_EXIT.load(Ordering::SeqCst) {
        thread::sleep(Duration::from_millis(250));
    }

    license.checkin();
    drop(manager);
    println!("license released");
    Ok(())
}

fn resolve_license_spec<I>(mut args: I) -> Result<String, String>
where
    I: Iterator<Item = String>,
{
    let mut license_spec = None;

    while let Some(arg) = args.next() {
        match arg.as_str() {
            "--license" | "-l" => {
                let value = args
                    .next()
                    .ok_or_else(|| "missing value after --license/-l".to_string())?;
                license_spec = Some(value);
            }
            _ if arg.starts_with("--license=") => {
                license_spec = Some(arg[10..].to_string());
            }
            _ if arg.starts_with('-') => {
                return Err(format!("unknown argument: {arg}"));
            }
            _ => {
                license_spec = Some(arg);
            }
        }
    }

    Ok(license_spec
        .or_else(|| env::var("RLM_LICENSE_SPEC").ok())
        .unwrap_or_else(|| "5053@localhost".to_string()))
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
