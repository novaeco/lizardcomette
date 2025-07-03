#![cfg_attr(not(test), no_std)]

pub mod config;
pub mod domain;
pub mod hardware;
pub mod network;
pub mod storage;
pub mod tasks;
pub mod ui;
pub mod utils;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_can_be_used() {
        let _ = config::AppConfig::default();
        domain::animals::ajouter();
        hardware::watchdog::reset();
        network::http::envoyer();
        storage::cache::vider();
        tasks::jobs::executer();
        ui::widgets::creer_bouton();
        utils::math::moyenne(1.0, 2.0);
    }
}
