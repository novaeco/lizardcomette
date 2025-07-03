//! Capteurs utilisés par le système.

use bme280::BME280;
use esp_idf_hal::i2c::{self, I2c};
use esp_idf_hal::prelude::*;

/// Initialise le capteur BME280 sur le bus I2C puis lit une mesure.
pub fn lire() -> bme280::Result<bme280::Measurements<I2c<'static>>> {
    let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();

    let i2c = i2c::Master::new(
        peripherals.i2c0,
        i2c::Pins {
            sda: peripherals.pins.gpio21,
            scl: peripherals.pins.gpio22,
        },
        i2c::config::Config::new().baudrate(Hertz(400_000)),
    )
    .unwrap();

    let mut bme280 = BME280::new_primary(i2c);
    bme280.init().ok();
    bme280.measure()
}

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
