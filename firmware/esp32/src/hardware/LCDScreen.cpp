#include "hardware/LCDScreen.h"

#include <Wire.h>

#include "config/PinConfig.h"

namespace {

constexpr unsigned long DOSE_CONFIRMED_DISPLAY_MS = 10UL * 1000UL;

}  // namespace

LCDScreen::LCDScreen(uint8_t address, uint8_t columns, uint8_t rows)
    : lcd(address, columns, rows),
      address(address),
      columns(columns),
      rows(rows),
      lastMessage(""),
      initialized(false),
      doseConfirmedVisible(false),
      doseConfirmedShownAtMs(0) {}

void LCDScreen::begin() {
    Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
    lcd.init();
    lcd.begin(columns, rows);
    lcd.backlight();
    initialized = true;
    showHomeScreen();
}

void LCDScreen::update() {
    if (!doseConfirmedVisible) {
        return;
    }

    if (millis() - doseConfirmedShownAtMs < DOSE_CONFIRMED_DISPLAY_MS) {
        return;
    }

    doseConfirmedVisible = false;
    showHomeScreen();
}

void LCDScreen::clear() {
    doseConfirmedVisible = false;
    lastMessage = "";
    printSerialScreen("");
    if (initialized) {
        lcd.clear();
    }
}

void LCDScreen::showReminder(const String& medicationName, int drawerId) {
    doseConfirmedVisible = false;
    lastMessage = String("Take ") + medicationName + " D" + String(drawerId);
    String drawerLine = String("Open drawer ") + String(drawerId);
    printSerialScreen(drawerLine, medicationName);
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, drawerLine);
    printLine(1, medicationName);
}

void LCDScreen::showMessage(const String& message) {
    doseConfirmedVisible = false;
    lastMessage = message;
    printSerialScreen(message);
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, message);
}

void LCDScreen::showDoseConfirmed() {
    doseConfirmedVisible = true;
    doseConfirmedShownAtMs = millis();
    lastMessage = "Dose confirmed";
    printSerialScreen("Dose confirmed");
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, "Dose confirmed");
    printLine(1, "");
}

void LCDScreen::showDoseMissed() {
    doseConfirmedVisible = false;
    lastMessage = "Dose missed";
    printSerialScreen("Dose missed");
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, "Dose missed");
    printLine(1, "");
}

void LCDScreen::showHomeScreen() {
    doseConfirmedVisible = false;
    lastMessage = "Smart Pill Box";
    printSerialScreen("Smart Pill Box");
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, "Smart Pill Box");
    printLine(1, "");
}

void LCDScreen::printLine(uint8_t row, const String& message) {
    if (row >= rows) {
        return;
    }

    String line = message.substring(0, columns);
    lcd.setCursor(0, row);
    lcd.print(line);
    for (int index = line.length(); index < columns; index++) {
        lcd.print(" ");
    }
}

void LCDScreen::printSerialScreen(const String& line1, const String& line2) {
    Serial.print("[LCD] ");
    Serial.print(line1);
    if (line2.length() > 0) {
        Serial.print(" | ");
        Serial.print(line2);
    }
    Serial.println();
}
