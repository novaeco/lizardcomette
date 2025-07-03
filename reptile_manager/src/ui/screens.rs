//! Gestion des écrans de l'interface.

/// Représente un écran générique.
///
/// # Examples
///
/// ```
/// use reptile_manager::ui::screens::Screen;
/// let screen = Screen::new("Accueil");
/// ```
pub struct Screen {
    name: &'static str,
}

impl Screen {
    /// Crée un nouvel [`Screen`].
    pub fn new(name: &'static str) -> Self {
        Self { name }
    }
}

/// Affiche l'écran principal.
pub fn afficher_principal() {
    // TODO: implémenter l'affichage principal
}

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
