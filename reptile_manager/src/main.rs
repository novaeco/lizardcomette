#![no_std]
#![no_main]
mod config;
mod domain;
mod hardware;
mod network;
mod storage;
mod tasks;
mod ui;
mod utils;

use esp_idf_hal::entry;
use tasks::spawner::spawn_tasks;

#[entry]
fn main() -> ! {
    // TODO: initialiser le matériel
    // Lance les tâches système une fois le matériel prêt
    spawn_tasks().expect("erreur lancement tâches");

    loop {}
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
