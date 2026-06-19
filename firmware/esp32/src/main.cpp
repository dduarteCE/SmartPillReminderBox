#include <Arduino.h>
#include <esp_log.h>

#include "controllers/DeviceController.h"

DeviceController device;

void setup() {
    Serial.begin(115200);
    delay(250);
    esp_log_level_set("vfs_api", ESP_LOG_NONE);
    Serial.println();
    Serial.println("[BOOT] Smart Pill Reminder Box starting");
    device.setup();
}

void loop() {
    device.loop();
}
