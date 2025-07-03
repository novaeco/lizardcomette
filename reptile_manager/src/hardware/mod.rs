//! Abstractions for all hardware peripherals.

pub mod actuators;
pub mod display;
pub mod sensors;
pub mod touch;
pub mod watchdog;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn modules_compile() {
        actuators::activer();
        let _ = display::Display::init();
        let _ = sensors::lire();
        let _ = touch::configurer();
        let _ = watchdog::init(1000);
    }
}
