#![no_std]
#![no_main]

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
