#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "config/SystemConfig.h"

class LCDScreen {
public:
    LCDScreen(
        uint8_t address = LCD_I2C_ADDRESS,
        uint8_t columns = LCD_COLUMNS,
        uint8_t rows = LCD_ROWS
    );

    void begin();
    void clear();
    void showReminder(const String& medicationName, int drawerId);
    void showMessage(const String& message);
    void showDoseConfirmed();
    void showDoseMissed();

private:
    void printLine(uint8_t row, const String& message);
    void printSerialScreen(const String& line1, const String& line2 = "");

    LiquidCrystal_I2C lcd;
    uint8_t address;
    uint8_t columns;
    uint8_t rows;
    String lastMessage;
    bool initialized;
};
