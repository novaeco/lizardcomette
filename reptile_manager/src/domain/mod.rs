//! Logique métier du gestionnaire de reptiles.

pub mod animals;
pub mod environment;
pub mod feeding;
pub mod health;
pub mod records;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_accessible() {
        animals::ajouter();
        environment::mettre_a_jour();
        feeding::planifier();
        health::enregistrer();
        let _ = records::Record {
            id: 0,
            notes: String::new(),
        };
    }
}
