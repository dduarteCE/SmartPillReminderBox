#pragma once

#include <Arduino.h>

class LCDScreen {
public:
    LCDScreen(uint8_t address = 0x27, uint8_t columns = 16, uint8_t rows = 2);

    void begin();
    void clear();
    void showReminder(const String& medicationName, int drawerId);
    void showMessage(const String& message);
    void showDoseConfirmed();
    void showDoseMissed();

private:
    uint8_t address;
    uint8_t columns;
    uint8_t rows;
    String lastMessage;
};
