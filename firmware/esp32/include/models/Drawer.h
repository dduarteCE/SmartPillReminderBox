#pragma once

#include <Arduino.h>
#include "hardware/LED.h"
#include "hardware/ReedSwitch.h"

class Drawer {
public:
    Drawer();
    Drawer(int id, const String& medicationName, bool enabled, int ledPin, int reedSwitchPin);

    void begin();
    void update();
    int getId() const;
    String getMedicationName() const;
    bool isEnabled() const;
    bool isOpen();
    void setMedicationName(const String& medicationName);
    void setEnabled(bool enabled);
    void highlight();
    void stopHighlight();

private:
    int id;
    String medicationName;
    bool enabled;
    LED led;
    ReedSwitch reedSwitch;
};
