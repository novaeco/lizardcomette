//! Serveur HTTP exposant les routes de l'application et intégrant MQTT.

use embedded_svc::http::Method;
use embedded_svc::mqtt::client::{Client, QoS};
use embedded_svc::io::Write;
use esp_idf_svc::http::server::{EspHttpServer, Configuration as HttpConfig};
use esp_idf_svc::mqtt::client::{EspMqttClient, MqttClientConfiguration};
use log::info;

/// Gère l'API haut niveau.
pub struct ApiServer {
    http: EspHttpServer<'static>,
    mqtt: EspMqttClient,
}

impl ApiServer {
    /// Crée une nouvelle instance d'[`ApiServer`].
    pub fn new() -> anyhow::Result<Self> {
        let http = EspHttpServer::new(&HttpConfig::default())?;
        let (mqtt, _connection) = EspMqttClient::new(
            "mqtt://broker.hivemq.com",
            &MqttClientConfiguration::default(),
        )?;

        Ok(Self { http, mqtt })
    }

    /// Lance le serveur HTTP et définit les routes.
    pub fn start(&mut self) -> anyhow::Result<()> {
        self.http.fn_handler("/status", Method::Get, |req| {
            info!("GET /status");
            req.into_ok_response()?.write_all(b"ok")?;
            Ok(())
        })?;

        self.http.fn_handler("/reptiles", Method::Get, |req| {
            info!("GET /reptiles");
            req.into_ok_response()?.write_all(b"[]")?;
            Ok(())
        })?;

        Ok(())
    }

    /// Publie un message via MQTT.
    pub fn publish_status(&self) -> anyhow::Result<()> {
        self.mqtt.publish("reptile/status", QoS::AtLeastOnce, false, b"ok")?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn api_server_new() {
        let _ = ApiServer::new();
    }
}
