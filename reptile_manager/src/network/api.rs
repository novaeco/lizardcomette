//! Serveur HTTP exposant les routes de l'application et intégrant MQTT.

/// Gère l'API haut niveau.
pub struct ApiServer {
    // TODO: stocker les gestionnaires HTTP/MQTT
}

impl ApiServer {
    /// Crée une nouvelle instance d'[`ApiServer`].
    pub fn new() -> Self {
        Self {
            // TODO: initialiser les gestionnaires
        }
    }

    /// Lance le serveur HTTP et définit les routes.
    pub fn start(&mut self) {
        // TODO: définir les routes HTTP
    }

    /// Publie un message via MQTT.
    pub fn publish_status(&self) {
        // TODO: publier l'état via MQTT
    }
}
