# lizardcomette

This project targets ESP32 devices. Upcoming versions will integrate over-the-air
updates with `EspOta`. Firmware information will be saved in NVS so the
bootloader can verify and roll back if needed. When encryption is enabled,
Wi-Fi credentials and other secrets will be stored in the encrypted NVS
partition.
