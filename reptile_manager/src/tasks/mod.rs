//! Gestionnaire de tâches de fond.

pub mod jobs;
pub mod scheduler;
pub mod spawner;
pub mod worker;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_compile() {
        jobs::executer();
        scheduler::lancer();
        let _ = spawner::spawn_tasks();
        worker::demarrer();
    }
}
