//! Communication réseau.

pub mod api;
pub mod http;
pub mod mqtt;
pub mod ota;
pub mod wifi;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_work() {
        let _ = api::ApiServer::new();
        http::envoyer();
        mqtt::publier();
        let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();
        let mut wifi = wifi::WifiManager::new(peripherals).unwrap();
        let _ = wifi.connect("", "");
        ota::start();
    }
}
