//! Logique métier du gestionnaire de reptiles.

pub mod animals;
pub mod environment;
pub mod feeding;
pub mod health;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_accessible() {
        animals::ajouter();
        environment::mettre_a_jour();
        feeding::planifier();
        health::enregistrer();
    }
}
