//! Fonctions d'aide pour les conversions de données.

/// Convertit des degrés Celsius en Fahrenheit.
pub fn celsius_en_fahrenheit(c: f32) -> f32 {
    // TODO: implémenter la conversion
    c * 1.8 + 32.0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn converts_celsius_to_fahrenheit() {
        let f = celsius_en_fahrenheit(0.0);
        assert_eq!(f, 32.0);
    }
}
