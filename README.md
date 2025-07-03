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

## Dependencies and features

`reptile_manager` relies on a few external crates to run on ESP32 hardware:

- [`pcf85063a`](https://crates.io/crates/pcf85063a) provides the RTC driver.
- [`chrono`](https://crates.io/crates/chrono) handles timestamp conversions. In
  embedded (`no_std`) environments the crate must be built with
  `default-features = false` and the `alloc` feature enabled:

```toml
chrono = { version = "0.4", default-features = false, features = ["alloc"] }
```

- [`xpt2046`](https://crates.io/crates/xpt2046) drives the SPI touch controller.
- [`bme280`](https://crates.io/crates/bme280) provides environmental sensing over I2C.
- [`onewire`](https://crates.io/crates/onewire) and [`ds18b20`](https://crates.io/crates/ds18b20) handle OneWire devices.

## Building with ESP-IDF

To compile and flash the firmware onto an ESP32, use the standard ESP-IDF workflow:

```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with the serial port attached to your board.

## OTA Updates

Once the device is connected to Wi‑Fi, firmware can be uploaded over the air. After building, run:

```bash
idf.py build
idf.py ota --ip <DEVICE_IP>
```

This uses `espota.py` under the hood to push the new binary.

## Architecture

```
          +---------+
          |   UI    |
          +---------+
               |
    +----------+------------+
    |          |            |
  Network   Hardware      Tasks
    |          |            |
  Storage    Config      Domain
               |
             Utils
```
