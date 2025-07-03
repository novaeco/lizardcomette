//! Création des tâches système.

use crate::hardware::watchdog;
use core::ffi::c_void;
use esp_idf_hal::{delay::FreeRtos, task};
use esp_idf_sys::{self as _, EspError};

extern "C" fn ui_task(_ptr: *mut c_void) {
    loop {
        watchdog::reset();
        // Appel périodique de la boucle LVGL
        unsafe {
            lvgl::sys::lv_timer_handler();
        }
        // TODO: ajuster la cadence en fonction de la charge
        FreeRtos::delay_ms(5);
    }
}

extern "C" fn sensor_task(_ptr: *mut c_void) {
    loop {
        watchdog::reset();
        // TODO: lire périodiquement les capteurs
        FreeRtos::delay_ms(100);
    }
}

extern "C" fn pid_task(_ptr: *mut c_void) {
    loop {
        watchdog::reset();
        // TODO: contrôler les actionneurs avec un PID
        FreeRtos::delay_ms(50);
    }
}

extern "C" fn network_task(_ptr: *mut c_void) {
    loop {
        watchdog::reset();
        // TODO: gérer le Wi-Fi et les appels API
        FreeRtos::delay_ms(200);
    }
}

/// Démarre toutes les tâches de fond du système.
pub fn spawn_tasks() -> Result<(), EspError> {
    unsafe {
        // TODO: définir les tailles de pile et priorités correctes
        task::create(
            ui_task,
            esp_idf_sys::cstr!("ui"),
            4096,
            core::ptr::null_mut(),
            5,
            None,
        )?;
        task::create(
            sensor_task,
            esp_idf_sys::cstr!("sensors"),
            4096,
            core::ptr::null_mut(),
            5,
            None,
        )?;
        task::create(
            pid_task,
            esp_idf_sys::cstr!("pid"),
            4096,
            core::ptr::null_mut(),
            5,
            None,
        )?;
        task::create(
            network_task,
            esp_idf_sys::cstr!("network"),
            4096,
            core::ptr::null_mut(),
            5,
            None,
        )?;
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn spawn_tasks_runs() {
        let _ = spawn_tasks();
    }
}
