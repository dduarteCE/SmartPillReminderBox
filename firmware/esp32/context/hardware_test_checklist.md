# ESP32 Hardware Test Checklist

This file is the board-level validation guide for the Smart Pill Reminder Box firmware.

Use it after flashing firmware to verify:

- pin mappings
- hardware modules
- constants in config files
- reminder flow behavior
- storage and connectivity assumptions

## 1. Pin Mapping To Validate

Source: [include/config/PinConfig.h](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/include/config/PinConfig.h)

Current configured pins:

- `LED_DRAWER_1_PIN = 12`
- `LED_DRAWER_2_PIN = 13`
- `LED_DRAWER_3_PIN = -1`
- `LED_DRAWER_4_PIN = -1`
- `LED_DRAWER_5_PIN = -1`
- `LED_DRAWER_6_PIN = -1`
- `LED_DRAWER_7_PIN = -1`
- `REED_DRAWER_1_PIN = 25`
- `REED_DRAWER_2_PIN = 26`
- `REED_DRAWER_3_PIN = -1`
- `REED_DRAWER_4_PIN = -1`
- `REED_DRAWER_5_PIN = -1`
- `REED_DRAWER_6_PIN = -1`
- `REED_DRAWER_7_PIN = -1`
- `BUZZER_PIN = 27`

Meaning:

- `-1` means that drawer hardware is not currently mapped to a real GPIO.
- Right now the firmware is effectively wired for drawers 1 and 2 only, plus the buzzer.

What to validate:

- drawer 1 LED is physically connected to GPIO 12
- drawer 2 LED is physically connected to GPIO 13
- drawer 1 reed switch is physically connected to GPIO 25
- drawer 2 reed switch is physically connected to GPIO 26
- buzzer is physically connected to GPIO 27
- drawer 3-7 are intentionally not wired yet, or their GPIO values must be updated

If the board wiring does not match these values, update `PinConfig.h`.

## 2. System Constants To Validate

Source: [include/config/SystemConfig.h](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/include/config/SystemConfig.h)

Current configured values:

- `MAX_DRAWERS = 7`
- `MAX_SCHEDULES = 10`
- `MAX_SCHEDULE_TIMES = 4`
- `MAX_DAYS_OF_WEEK = 7`
- `MAX_PENDING_EVENTS = 10`
- `REMINDER_INTERACTION_WINDOW_MS = 300000`
- `CONFIG_FILE_PATH = "/config.json"`
- `DEVICE_AP_SSID = "SmartPillBox"`
- `DEVICE_AP_PASSWORD = "smartpill123"`
- `WEBSOCKET_PORT = 81`
- `LCD_I2C_ADDRESS = 0x27`
- `LCD_COLUMNS = 16`
- `LCD_ROWS = 2`

What to validate:

- the LCD really uses I2C address `0x27`
- the LCD is actually `16x2`
- `REMINDER_INTERACTION_WINDOW_MS` matches the intended timeout window
- the AP SSID and password are acceptable for testing
- the mobile or web client connects to WebSocket port `81`
- `MAX_PENDING_EVENTS` is large enough for the expected offline buffer

If the LCD does not respond, the most likely first thing to check is the I2C address.

## 3. Flash And Boot Validation

After flashing:

- the board boots without reset loops
- LittleFS mounts successfully
- the device creates the `SmartPillBox` Wi-Fi access point
- the web server starts
- the WebSocket server starts

What to watch for:

- repeated boot failures can suggest bad wiring, bad power, or a GPIO conflict
- storage failures suggest a LittleFS issue
- AP not appearing suggests Wi-Fi init failure or boot not reaching normal setup

## 4. LCD Validation

Source: [src/hardware/LCDScreen.cpp](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/src/hardware/LCDScreen.cpp)

Expected behavior:

- on startup the LCD shows `Smart Pill Box`
- on reminder start it shows medication name on line 1
- on reminder start it shows `Drawer X` on line 2
- on dose completion it shows `Dose confirmed`
- on missed dose it shows `Dose missed`

What to validate:

- the LCD powers on
- the LCD backlight turns on
- text is visible and not garbled
- old characters do not remain on screen after shorter messages
- long medication names are truncated cleanly to fit the display width

If the LCD is blank:

- check power and ground
- check SDA and SCL wiring
- check the I2C address
- confirm the I2C backpack is compatible with `LiquidCrystal_I2C`

## 5. Buzzer Validation

Source: [src/hardware/Buzzer.cpp](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/src/hardware/Buzzer.cpp)

Expected behavior:

- buzzer activates when a reminder starts
- buzzer deactivates when the dose is completed
- buzzer deactivates when the dose times out and becomes missed

What to validate:

- buzzer sounds on reminder start
- buzzer stops after successful drawer interaction
- buzzer stops after timeout
- buzzer does not remain stuck on between reminders

If behavior is inverted, check buzzer type and wiring.

## 6. Drawer LED Validation

Source: [src/controllers/DeviceController.cpp](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/src/controllers/DeviceController.cpp:187)

Expected behavior:

- correct drawer LED starts blinking when reminder begins
- correct drawer LED stops when dose is completed
- correct drawer LED stops when dose is missed

What to validate:

- drawer 1 schedules blink only drawer 1 LED
- drawer 2 schedules blink only drawer 2 LED
- wrong drawer LEDs do not blink
- LED stops after reminder is resolved

If LEDs do not blink:

- check pin mapping
- check LED polarity and resistor wiring
- confirm the drawer is one of the mapped drawers

## 7. Reed Switch Validation

Expected behavior:

- opening the correct drawer during an active reminder generates `DRAWER_OPENED`
- closing the same drawer generates `DRAWER_CLOSED`
- closing after opening completes the dose

What to validate:

- drawer closed state is read correctly at idle
- drawer open state changes when magnet moves away
- no false triggers happen when drawer remains closed
- the right drawer completes the reminder sequence

If readings seem reversed:

- inspect the reed switch wiring
- inspect pull-up or pull-down assumptions in the `ReedSwitch` implementation

## 8. Clock Validation

Source: [src/hardware/ClockModule.cpp](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/src/hardware/ClockModule.cpp)

Current clock behavior:

- time is software-based, not RTC-based
- time starts advancing only after the app calls the HTTP set-time endpoint
- time is not persisted across reboot

What to validate:

- after calling the set-time endpoint, the returned time is correct
- one or two minutes later, reported time has advanced
- day rollover works around midnight
- day-of-week rollover works after midnight
- schedules do not trigger before time is configured

Important limitation:

- after reboot, time must be set again by the client

## 9. Reminder Flow Validation

Expected end-to-end behavior:

1. schedule exists for an enabled drawer
2. device time reaches that schedule
3. correct drawer LED starts blinking
4. buzzer turns on
5. LCD shows medication reminder
6. opening drawer emits `DRAWER_OPENED`
7. closing drawer emits `DRAWER_CLOSED`
8. dose completes with `DOSE_COMPLETED`
9. buzzer stops
10. LED stops
11. completion message appears on LCD

Timeout path:

1. schedule triggers
2. user does nothing
3. after `REMINDER_INTERACTION_WINDOW_MS`, event becomes `DOSE_MISSED`
4. buzzer stops
5. LED stops
6. LCD shows missed message

## 10. HTTP API Validation

Source: [context/esp32_api_contract.md](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/context/esp32_api_contract.md)

What to validate:

- create or update drawer configuration successfully
- create schedule successfully
- get schedules successfully
- set time successfully
- get current time successfully
- acknowledge events successfully

Important behavior to test:

- drawers persisted to storage survive reboot
- schedules persisted to storage survive reboot
- unacknowledged events survive reboot until acknowledged

## 11. WebSocket Validation

What to validate:

- client connects successfully to the configured WebSocket port
- reminder events are broadcast live
- disconnecting a client does not crash the device
- reconnecting still receives later events

Events to observe:

- `REMINDER_STARTED`
- `DRAWER_OPENED`
- `DRAWER_CLOSED`
- `DOSE_COMPLETED`
- `DOSE_MISSED`

## 12. Storage Validation

Source: [src/storage/StorageManager.cpp](/home/dadump/Documents/Local_Projects/SmartPillReminderBox/firmware/esp32/src/storage/StorageManager.cpp)

What to validate:

- `config.json` is created in LittleFS when needed
- saved drawers reload correctly after reboot
- saved schedules reload correctly after reboot
- saved events reload correctly after reboot
- acknowledged events are removed correctly

Possible failure symptoms:

- missing config after reboot
- corrupted JSON after several writes
- stale events reappearing after acknowledgment

## 13. Practical Board Test Sequence

Recommended order:

1. flash firmware and confirm clean boot
2. confirm Wi-Fi AP appears
3. confirm LCD powers on and prints startup text
4. confirm buzzer can activate during a reminder
5. confirm drawer 1 LED and reed switch work
6. confirm drawer 2 LED and reed switch work
7. call set-time endpoint
8. create one drawer config and one near-future schedule
9. wait for reminder to trigger
10. validate LED, buzzer, LCD, and WebSocket event
11. open and close the correct drawer
12. validate completion event and output shutdown
13. reboot device and confirm persisted config reloads

## 14. Likely Constants You May Need To Adjust

Most likely to change during bring-up:

- `LCD_I2C_ADDRESS`
- `LCD_COLUMNS`
- `LCD_ROWS`
- `BUZZER_PIN`
- `LED_DRAWER_*_PIN`
- `REED_DRAWER_*_PIN`
- `REMINDER_INTERACTION_WINDOW_MS`
- `DEVICE_AP_SSID`
- `DEVICE_AP_PASSWORD`
- `WEBSOCKET_PORT`

## 15. Known Current Constraints

Current firmware constraints to keep in mind:

- only drawers with real GPIO mappings can be tested meaningfully
- the clock is not RTC-backed
- the clock must be set again after reset
- WebSocket runs on port `81`
- LCD support assumes a `LiquidCrystal_I2C` compatible module

