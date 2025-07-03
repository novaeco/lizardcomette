//! Actionneurs contrôlés par le système.

use ds18b20::Ds18b20;
use esp_idf_hal::gpio::PinDriver;
use esp_idf_hal::prelude::*;
use onewire::OneWire;

/// Initialise le bus OneWire et active un actionneur.
pub fn activer() {
    let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();

    let pin = PinDriver::input_output(peripherals.pins.gpio4).unwrap();
    let mut one = OneWire::new(pin).unwrap();

    // Exemple : recherche d'un périphérique OneWire, ici un capteur DS18B20
    if let Ok(Some(device)) = one.reset_search().next() {
        let mut sensor = Ds18b20::new(device).unwrap();
        let _ = sensor.start_temp_measurement(&mut one);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn activer_compiles() {
        activer();
    }
}
