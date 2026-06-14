# ESP32 Firmware

This directory is a PlatformIO project for the Smart Pill Reminder Box ESP32 firmware.

## Layout

```text
firmware/esp32/
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
├── data/
│   └── config.json
└── test/
```

## Commands

Run these from `firmware/esp32`:

```bash
pio run
pio run --target upload
pio device monitor
```

The project uses the PlatformIO `espressif32` platform with the Arduino framework.
