//! Navigation entre les différentes vues.

use super::screens::{self, Screen};

/// Gestionnaire de navigation circulaire entre plusieurs écrans.
pub struct Navigator {
    screens: [Screen; 5],
    index: usize,
}

impl Navigator {
    /// Crée une nouvelle instance contenant l'ensemble des écrans connus.
    pub const fn new() -> Self {
        Self {
            screens: screens::tous(),
            index: 0,
        }
    }

    /// Retourne l'écran courant.
    pub fn courant(&self) -> &Screen {
        &self.screens[self.index]
    }

    /// Avance vers l'écran suivant en boucle.
    pub fn suivant(&mut self) {
        self.index = (self.index + 1) % self.screens.len();
    }
}

/// Passe à l'écran suivant en utilisant un [`Navigator`] global.
pub fn suivant() {
    use core::cell::RefCell;

    thread_local! {
        static NAV: RefCell<Navigator> = RefCell::new(Navigator::new());
    }

    NAV.with(|nav| nav.borrow_mut().suivant());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn navigator_cycle() {
        let mut nav = Navigator::new();
        nav.suivant();
        assert_eq!(nav.index, 1);
    }
}
