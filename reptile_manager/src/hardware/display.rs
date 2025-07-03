//! Gestion de l'affichage du système.

use esp_idf_hal::prelude::*;
use esp_idf_hal::spi::{self, Master};
use lvgl::display::Flush;
use lvgl::prelude::*;

/// Représente l'écran relié en SPI.
pub struct Display {
    spi: Master<
        spi::SPI2,
        esp_idf_hal::gpio::Gpio18<esp_idf_hal::gpio::Output>,
        esp_idf_hal::gpio::Gpio23<esp_idf_hal::gpio::Output>,
        esp_idf_hal::gpio::Gpio19<esp_idf_hal::gpio::Input>,
        esp_idf_hal::gpio::Gpio5<esp_idf_hal::gpio::Output>,
    >,
}

impl Display {
    /// Initialise l'affichage et retourne la structure [`Display`].
    pub fn init() -> Self {
        // Récupère les périphériques pour configurer les broches SPI
        let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();

        let spi = Master::new(
            peripherals.spi2,
            spi::Pins {
                sclk: peripherals.pins.gpio18,
                sdo: peripherals.pins.gpio23,
                sdi: Some(peripherals.pins.gpio19),
                cs: Some(peripherals.pins.gpio5),
            },
            spi::config::Config::default(),
        )
        .unwrap();

        // TODO: modifier la résolution ici pour prendre en charge
        //        d'autres tailles d'écrans
        lvgl::init();

        unsafe { lvgl::display::set_flush_callback(Some(flush_cb)) };

        Self { spi }
    }
}

impl Flush for Display {
    fn flush(&mut self, disp_drv: &mut lv_disp_drv_t, area: &lv_area_t, color: *const lv_color_t) {
        unsafe { flush_cb(disp_drv, area, color) }
    }
}

/// Callback utilisé par LVGL pour envoyer une zone de pixels au contrôleur.
pub unsafe extern "C" fn flush_cb(
    disp_drv: &mut lv_disp_drv_t,
    area: &lv_area_t,
    color: *const lv_color_t,
) {
    // TODO: transmettre les pixels via SPI
    lvgl::disp_flush_ready(disp_drv);
}

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
