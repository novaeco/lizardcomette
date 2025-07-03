//! Mise à jour OTA du firmware.

use esp_idf_sys as sys;

/// Déclenche une mise à jour OTA.
pub fn start() {
    // TODO: utiliser esp_ota pour appliquer un nouveau firmware
    unsafe {
        sys::esp_task_wdt_reset(); // marque d'activité pendant la mise à jour
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn start_runs() {
        start();
    }
}
