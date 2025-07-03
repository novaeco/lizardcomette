//! Fonctions utilitaires diverses.

pub mod conversions;
pub mod errors;
pub mod logging;
pub mod math;
pub mod time;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn moyenne_works_via_mod() {
        assert_eq!(math::moyenne(1.0, 3.0), 2.0);
    }
}
