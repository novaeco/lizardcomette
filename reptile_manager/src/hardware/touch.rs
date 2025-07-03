//! Gestion du contrôleur tactile.

use esp_idf_hal::prelude::*;
use esp_idf_hal::spi::{self, Master};
use xpt2046::{Orientation, Xpt2046};

/// Configure le contrôleur tactile et retourne le driver [`Xpt2046`].
pub fn configurer() -> Xpt2046<
    Master<
        spi::SPI3,
        esp_idf_hal::gpio::Gpio25<esp_idf_hal::gpio::Output>,
        esp_idf_hal::gpio::Gpio26<esp_idf_hal::gpio::Output>,
        esp_idf_hal::gpio::Gpio27<esp_idf_hal::gpio::Input>,
        esp_idf_hal::gpio::Gpio33<esp_idf_hal::gpio::Output>,
    >,
    esp_idf_hal::gpio::Gpio33<esp_idf_hal::gpio::Output>,
    esp_idf_hal::gpio::Gpio32<esp_idf_hal::gpio::Input>,
> {
    // Récupère les périphériques pour configurer les broches SPI
    let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();

    let spi = Master::new(
        peripherals.spi3,
        spi::Pins {
            sclk: peripherals.pins.gpio25,
            sdo: peripherals.pins.gpio26,
            sdi: Some(peripherals.pins.gpio27),
            cs: Some(peripherals.pins.gpio33),
        },
        spi::config::Config::default(),
    )
    .unwrap();

    let irq = peripherals.pins.gpio32;
    let cs = peripherals.pins.gpio33;

    Xpt2046::new(spi, cs, irq, Orientation::Portrait)
}

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
