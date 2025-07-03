//! Gestion du watchdog (TWDT).

use core::ptr;
use esp_idf_sys::{self as sys, EspError};

/// Initialise le watchdog avec le timeout spécifié en millisecondes.
pub fn init(timeout_ms: u32) -> Result<(), EspError> {
    let config = sys::esp_task_wdt_config_t {
        timeout_ms,
        idle_core_mask: 0,
        trigger_panic: false,
    };
    unsafe {
        sys::esp!(sys::esp_task_wdt_init(&config))?;
        sys::esp!(sys::esp_task_wdt_add(ptr::null_mut()))?;
    }
    Ok(())
}

/// Réinitialise le watchdog pour la tâche courante.
pub fn reset() {
    unsafe {
        sys::esp_task_wdt_reset();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn init_and_reset_compile() {
        let _ = init(1000);
        reset();
    }
}
