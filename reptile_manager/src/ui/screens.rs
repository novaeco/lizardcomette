//! Gestion des écrans de l'interface.

/// Types d'écrans gérés par l'application.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum ScreenKind {
    Accueil,
    Capteurs,
    Graphiques,
    Reglages,
    Apropos,
}

/// Représente un écran générique avec un nom et un type.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Screen {
    name: &'static str,
    kind: ScreenKind,
}

impl Screen {
    /// Crée un nouvel [`Screen`].
    pub const fn new(name: &'static str, kind: ScreenKind) -> Self {
        Self { name, kind }
    }

    /// Retourne le nom de l'écran.
    pub const fn name(&self) -> &'static str {
        self.name
    }

    /// Retourne le type d'écran.
    pub const fn kind(&self) -> ScreenKind {
        self.kind
    }
}

/// Construit l'écran d'accueil.
pub const fn accueil() -> Screen {
    Screen::new("Accueil", ScreenKind::Accueil)
}

/// Construit l'écran listant les capteurs.
pub const fn capteurs() -> Screen {
    Screen::new("Capteurs", ScreenKind::Capteurs)
}

/// Construit l'écran affichant les graphiques.
pub const fn graphiques() -> Screen {
    Screen::new("Graphiques", ScreenKind::Graphiques)
}

/// Construit l'écran de réglages.
pub const fn reglages() -> Screen {
    Screen::new("Réglages", ScreenKind::Reglages)
}

/// Construit l'écran d'information sur l'application.
pub const fn apropos() -> Screen {
    Screen::new("À propos", ScreenKind::Apropos)
}

/// Retourne la liste de tous les écrans dans l'ordre de navigation.
pub const fn tous() -> [Screen; 5] {
    [accueil(), capteurs(), graphiques(), reglages(), apropos()]
}

/// Affiche l'écran principal. Dans ce squelette l'implémentation se limite à
/// un journalisation via [`log`].
pub fn afficher_principal() {
    log::info!("Affichage de l'écran: {}", accueil().name());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn screens_order() {
        let all = tous();
        assert_eq!(all.len(), 5);
        assert_eq!(all[0].name(), "Accueil");
    }
}
