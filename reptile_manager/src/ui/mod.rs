//! Interface utilisateur graphique.

pub mod navigation;
pub mod screens;
pub mod theme;
pub mod widgets;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_accessible() {
        navigation::suivant();
        screens::afficher_principal();
        theme::appliquer();
        widgets::creer_bouton();
    }
}
