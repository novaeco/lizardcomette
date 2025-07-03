//! Gestionnaire de tâches de fond.

pub mod jobs;
pub mod scheduler;
pub mod spawner;
pub mod worker;

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
