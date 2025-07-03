# lizardcomette

This project targets ESP32 devices. Upcoming versions will integrate over-the-air
updates with `EspOta`. Firmware information will be saved in NVS so the
bootloader can verify and roll back if needed. When encryption is enabled,
Wi-Fi credentials and other secrets will be stored in the encrypted NVS
partition.

## Utilities

Utility modules live under `reptile_manager/src/utils`.

- `errors.rs` re-exports [`anyhow::Error`](https://docs.rs/anyhow) and
  `anyhow::Result` so you can simply `use crate::utils::errors::*;`.
- `time.rs` provides the `Rtc` struct wrapping a PCF85063 I2C driver. It exposes
  two helper methods:
  - `now()` to read the current time from the RTC
  - `set_time()` to update the clock

These modules are publicly exported in `utils::` and can be used across the
project with `use crate::utils::time::Rtc;` for example.
