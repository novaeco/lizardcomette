#![no_std]
#![no_main]

use esp_idf_hal::entry;
use reptile_manager::hardware::watchdog;
use reptile_manager::tasks::spawner::spawn_tasks;

#[entry]
fn main() -> ! {
    // TODO: initialiser le matériel
    watchdog::init(5000).expect("initialisation watchdog");
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
