//! Gestion du thème de l'application.

use core::cell::Cell;

/// Représente les couleurs et polices utilisées par l'interface.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Theme {
    /// Couleur de fond principale sous forme RVB 0xRRGGBB.
    pub background: u32,
    /// Couleur de texte principale sous forme RVB 0xRRGGBB.
    pub foreground: u32,
    /// Police utilisée pour le texte courant.
    pub font_small: &'static str,
    /// Police utilisée pour les titres.
    pub font_large: &'static str,
}

impl Theme {
    /// Crée un nouveau thème avec les paramètres donnés.
    pub const fn new(
        background: u32,
        foreground: u32,
        font_small: &'static str,
        font_large: &'static str,
    ) -> Self {
        Self {
            background,
            foreground,
            font_small,
            font_large,
        }
    }

    /// Retourne un thème par défaut.
    pub const fn default() -> Self {
        Self::new(0x000000, 0xFFFFFF, "sans", "sans-bold")
    }
}

/// Thème courant stocké dans une cellule globale.
static CURRENT_THEME: Cell<Option<Theme>> = Cell::new(None);

/// Applique le thème courant. S'il n'y a pas de thème défini, le thème par
/// défaut est utilisé. L'application concrète dépend de LVGL et n'est pas
/// implémentée ici.
pub fn appliquer() {
    let theme = CURRENT_THEME.get().unwrap_or_else(|| {
        let d = Theme::default();
        CURRENT_THEME.set(Some(d));
        d
    });

    // Dans un projet réel, on configurerait ici les styles LVGL à partir des
    // couleurs et polices du thème. Les appels sont laissés de côté afin de
    // conserver ce module indépendant du matériel.
    let _ = theme;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn apply_sets_default() {
        appliquer();
    }
}
