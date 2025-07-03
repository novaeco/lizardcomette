//! Gestion du module Wi-Fi.

/// Structure simplifiée gérant la connexion Wi-Fi.
pub struct WifiManager {
    // TODO: stocker le pilote Wi-Fi ou les paramètres nécessaires
}

impl WifiManager {
    /// Crée un nouveau [`WifiManager`].
    pub fn new() -> Self {
        Self {
            // TODO: initialiser les champs
        }
    }

    /// Connecte l'appareil au réseau.
    pub fn connect(&mut self) {
        // TODO: implémenter la connexion Wi-Fi
    }

    /// Callback lorsque la connexion est établie.
    pub fn on_connected(&mut self) {
        // TODO: réagir à la connexion
    }

    /// Callback lorsque la connexion est perdue.
    pub fn on_disconnected(&mut self) {
        // TODO: réagir à la déconnexion
    }
}
