//! Gestion du module Wi-Fi.

use embedded_svc::wifi::{ClientConfiguration, Configuration, Wifi};
use esp_idf_hal::peripherals::Peripherals;
use esp_idf_svc::eventloop::{EspEventLoop, EspSubscription, System};
use esp_idf_svc::nvs::EspDefaultNvsPartition;
use esp_idf_svc::wifi::{EspWifi, WifiEvent};
use log::{info, warn};

/// Structure simplifiée gérant la connexion Wi-Fi et les événements.
pub struct WifiManager<'a> {
    wifi: EspWifi<'a>,
    _subscription: EspSubscription<'a, System>,
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

        Ok(Self { wifi, _subscription: subscription })
    }

    /// Connecte l'appareil au réseau.
    pub fn connect(&mut self, ssid: &str, password: &str) -> anyhow::Result<()> {
        self.wifi.set_configuration(&Configuration::Client(ClientConfiguration {
            ssid: ssid.into(),
            password: password.into(),
            ..Default::default()
        }))?;
        self.wifi.start()?;
        self.wifi.connect()?;
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
