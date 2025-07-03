//! Gestion du module Wi-Fi.

use anyhow::Result;
use embedded_svc::wifi::{ClientConfiguration, Configuration, Wifi};
use esp_idf_hal::peripherals::Peripherals;
use esp_idf_svc::eventloop::{EspEventLoop, EspSubscription, System};
use esp_idf_svc::nvs::{EspDefaultNvsPartition, EspNvs};
use esp_idf_svc::wifi::{EspWifi, WifiEvent};
use log::{info, warn};

/// Structure simplifiée gérant la connexion Wi-Fi et les événements.
pub struct WifiManager<'a> {
    wifi: EspWifi<'a>,
    _subscription: EspSubscription<'a, System>,
}

/// Sauvegarde les identifiants dans la NVS.
fn save_credentials(ssid: &str, password: &str) -> Result<()> {
    let part = EspDefaultNvsPartition::take()?;
    let nvs = EspNvs::new(part, "wifi", true)?;
    nvs.set_str("ssid", ssid)?;
    nvs.set_str("password", password)?;
    Ok(())
}

/// Lit les identifiants stockés dans la NVS.
fn load_credentials() -> Result<Option<(String, String)>> {
    let part = EspDefaultNvsPartition::take()?;
    let nvs = EspNvs::new(part, "wifi", true)?;
    match (nvs.get_str("ssid")?, nvs.get_str("password")?) {
        (Some(s), Some(p)) => Ok(Some((s, p))),
        _ => Ok(None),
    }
}

impl WifiManager {
    /// Crée un nouveau [`WifiManager`].
    pub fn new(peripherals: Peripherals) -> anyhow::Result<Self> {
        let sysloop = EspEventLoop::take()?;
        let nvs = EspDefaultNvsPartition::take()?;
        let mut wifi = EspWifi::new(peripherals.modem, sysloop.clone(), Some(nvs))?;

        let subscription = sysloop.subscribe::<WifiEvent, _>(|event| match event {
            WifiEvent::StaConnected => info!("Wi-Fi connecté"),
            WifiEvent::StaDisconnected => warn!("Wi-Fi déconnecté"),
            _ => {}
        })?;

        Ok(Self {
            wifi,
            _subscription: subscription,
        })
    }

    /// Connecte l'appareil au réseau.
    pub fn connect(&mut self, ssid: &str, password: &str) -> anyhow::Result<()> {
        self.wifi
            .set_configuration(&Configuration::Client(ClientConfiguration {
                ssid: ssid.into(),
                password: password.into(),
                ..Default::default()
            }))?;
        self.wifi.start()?;
        self.wifi.connect()?;
        save_credentials(ssid, password)?;
        Ok(())
    }

    /// Tente une reconnexion en utilisant les identifiants en mémoire.
    pub fn reconnect(&mut self) -> anyhow::Result<()> {
        if self.wifi.is_connected().unwrap_or(false) {
            return Ok(());
        }
        if let Some((ssid, password)) = load_credentials()? {
            self.connect(&ssid, &password)?;
        }
        Ok(())
    }

    /// Callback lorsque la connexion est établie.
    pub fn on_connected(&mut self) {
        info!("Connexion Wi-Fi établie");
    }

    /// Callback lorsque la connexion est perdue.
    pub fn on_disconnected(&mut self) {
        warn!("Connexion Wi-Fi perdue");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use esp_idf_hal::peripherals::Peripherals;

    #[test]
    fn wifi_manager_new() {
        let peripherals = Peripherals::take().unwrap();
        let _ = WifiManager::new(peripherals);
    }
}
