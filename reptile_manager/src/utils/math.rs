//! Fonctions mathématiques supplémentaires.

/// Calcule la moyenne de deux nombres.
pub fn moyenne(a: f32, b: f32) -> f32 {
    // TODO: calculer la moyenne
    (a + b) / 2.0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn moyenne_calculation() {
        let result = moyenne(2.0, 4.0);
        assert_eq!(result, 3.0);
    }
}
