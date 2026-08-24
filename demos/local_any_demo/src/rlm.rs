use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};

const RLM_ERRSTRING_MAX: usize = 512;

type RlmHandleRaw = *mut c_void;
type RlmLicenseRaw = *mut c_void;

unsafe extern "C" {
    fn rlm_init(
        license_path: *const c_char,
        app_path: *const c_char,
        isv: *const c_char,
    ) -> RlmHandleRaw;
    fn rlm_stat(handle: RlmHandleRaw) -> c_int;
    fn rlm_close(handle: RlmHandleRaw) -> c_int;
    fn rlm_checkout(
        handle: RlmHandleRaw,
        product: *const c_char,
        version: *const c_char,
        count: c_int,
    ) -> RlmLicenseRaw;
    fn rlm_checkin(license: RlmLicenseRaw) -> c_int;
    fn rlm_license_stat(license: RlmLicenseRaw) -> c_int;
    fn rlm_errstring(
        license: RlmLicenseRaw,
        handle: RlmHandleRaw,
        buffer: *mut c_char,
    ) -> *mut c_char;
}

pub struct LicenseManager {
    handle: RlmHandleRaw,
}

pub struct CheckedOutLicense {
    license: RlmLicenseRaw,
}

impl LicenseManager {
    pub fn new(license_spec: &str, app_path: &str) -> Result<Self, String> {
        let license_spec =
            CString::new(license_spec).map_err(|_| "license spec contains NUL byte")?;
        let app_path = CString::new(app_path).map_err(|_| "app path contains NUL byte")?;
        let empty = CString::new("").expect("CString::new should not fail for empty string");

        let handle = unsafe { rlm_init(license_spec.as_ptr(), app_path.as_ptr(), empty.as_ptr()) };
        if handle.is_null() {
            return Err("rlm_init returned a null handle".to_string());
        }

        let status = unsafe { rlm_stat(handle) };
        if status != 0 {
            let message = format!("rlm_init failed with status {status}");
            unsafe {
                let _ = rlm_close(handle);
            }
            return Err(message);
        }

        Ok(Self { handle })
    }

    pub fn checkout(
        &self,
        product: &str,
        version: &str,
        count: i32,
    ) -> Result<CheckedOutLicense, String> {
        let product = CString::new(product).map_err(|_| "product contains NUL byte")?;
        let version = CString::new(version).map_err(|_| "version contains NUL byte")?;

        let license =
            unsafe { rlm_checkout(self.handle, product.as_ptr(), version.as_ptr(), count) };
        if license.is_null() {
            return Err("rlm_checkout returned a null license handle".to_string());
        }

        let status = unsafe { rlm_license_stat(license) };
        if status != 0 {
            return Err(self.error_string(license));
        }

        Ok(CheckedOutLicense { license })
    }

    fn error_string(&self, license: RlmLicenseRaw) -> String {
        let mut buffer = vec![0_i8; RLM_ERRSTRING_MAX];
        let ptr = unsafe { rlm_errstring(license, self.handle, buffer.as_mut_ptr()) };

        if ptr.is_null() {
            return "unknown RLM error".to_string();
        }

        unsafe { CStr::from_ptr(ptr) }
            .to_string_lossy()
            .into_owned()
    }
}

impl Drop for LicenseManager {
    fn drop(&mut self) {
        unsafe {
            let _ = rlm_close(self.handle);
        }
    }
}

impl CheckedOutLicense {
    pub fn checkin(self) {
        drop(self);
    }
}

impl Drop for CheckedOutLicense {
    fn drop(&mut self) {
        unsafe {
            let _ = rlm_checkin(self.license);
        }
    }
}
