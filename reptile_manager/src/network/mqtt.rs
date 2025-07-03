//! Support du protocole MQTT avec reconnexion automatique.

use anyhow::Result;
use embedded_svc::mqtt::client::{Client, Event, MqttClient, QoS};
use esp_idf_svc::mqtt::client::{EspMqttClient, MqttClientConfiguration};
use log::{info, warn};
use std::sync::mpsc::{channel, Sender};

/// Gestionnaire simple pour un client MQTT.
pub struct MqttService {
    client: EspMqttClient,
    _events_task: Sender<()>,
}

impl MqttService {
    /// Initialise le client et lance la boucle d\'écoute des évènements.
    pub fn new(uri: &str, client_id: &str) -> Result<Self> {
        let (client, mut connection) = EspMqttClient::new(
            uri,
            &MqttClientConfiguration {
                client_id: Some(client_id),
                ..Default::default()
            },
        )?;

        // Petite tâche pour consommer les évènements et détecter les déconnexions.
        let (tx, rx) = channel();
        std::thread::spawn(move || {
            for event in connection.iter() {
                match event {
                    Ok(Event::Connected(_)) => info!("MQTT connecté"),
                    Ok(Event::Disconnected(_)) => warn!("MQTT déconnecté"),
                    Ok(_) => {}
                    Err(e) => warn!("Erreur MQTT: {:?}", e),
                }
                if rx.try_recv().is_ok() {
                    break;
                }
            }
        });

        Ok(Self { client, _events_task: tx })
    }

    /// S\'enregistre sur un topic.
    pub fn register(&mut self, topic: &str) -> Result<()> {
        self.client.subscribe(topic, QoS::AtLeastOnce)?;
        Ok(())
    }

    /// Publie un message sur un topic.
    pub fn publish(&mut self, topic: &str, data: &[u8]) -> Result<()> {
        self.client.publish(topic, QoS::AtLeastOnce, false, data)?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn mqtt_service_new() {
        let _ = MqttService::new("mqtt://localhost", "test");
    }
}
