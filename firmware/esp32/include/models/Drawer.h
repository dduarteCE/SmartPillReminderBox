#pragma once

#include <Arduino.h>
#include "hardware/LED.h"
#include "hardware/ReedSwitch.h"

class Drawer {
public:
    Drawer();
    Drawer(
        int id,
        const String& medicationName,
        bool enabled,
        int ledPin,
        int reedSwitchPin,
        int pillCount = 0
    );

    void begin();
    void update();
    int getId() const;
    String getMedicationName() const;
    bool isEnabled() const;
    int getPillCount() const;
    bool isOpen();
    void setMedicationName(const String& medicationName);
    void setEnabled(bool enabled);
    void setPillCount(int pillCount);
    bool recordDoseTaken();
    void highlight();
    void stopHighlight();

private:
    int id;
    String medicationName;
    bool enabled;
    int pillCount;
    LED led;
    ReedSwitch reedSwitch;
};
