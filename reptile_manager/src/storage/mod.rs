//! Gestion du stockage des données.

pub mod cache;
pub mod database;
pub mod filesystem;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_run() {
        cache::vider();
        database::ouvrir();
        let _ = filesystem::exemple_json("/tmp/test.json");
    }
}
