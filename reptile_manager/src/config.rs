//! Paramètres de configuration de l'application.

pub mod settings;

pub use settings::charger;

use serde::{Deserialize, Serialize};

/// Configuration globale de l'application.
#[derive(Debug, Default, Serialize, Deserialize)]
pub struct AppConfig {
    /// Active le mode debug.
    pub debug: bool,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn app_config_default() {
        let cfg = AppConfig::default();
        assert!(!cfg.debug);
    }
}
