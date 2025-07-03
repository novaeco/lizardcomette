#![no_std]
#![no_main]
mod config;
mod domain;
mod hardware;
mod network;
mod storage;
mod tasks;
mod ui;
mod utils;

use esp_idf_hal::entry;

#[entry]
fn main() -> ! {
    loop {}
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
