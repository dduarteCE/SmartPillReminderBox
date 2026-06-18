#include <Arduino.h>
#include "controllers/DeviceController.h"

DeviceController device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
