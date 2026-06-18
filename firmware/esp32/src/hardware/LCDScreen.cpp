#include "hardware/LCDScreen.h"

LCDScreen::LCDScreen(uint8_t address, uint8_t columns, uint8_t rows)
    : lcd(address, columns, rows),
      address(address),
      columns(columns),
      rows(rows),
      lastMessage(""),
      initialized(false) {}

void LCDScreen::begin() {
    lcd.init();
    lcd.backlight();
    initialized = true;
    showMessage("Smart Pill Box");
}

void LCDScreen::clear() {
    lastMessage = "";
    printSerialScreen("");
    if (initialized) {
        lcd.clear();
    }
}

void LCDScreen::showReminder(const String& medicationName, int drawerId) {
    lastMessage = String("Take ") + medicationName + " D" + String(drawerId);
    printSerialScreen(medicationName, String("Drawer ") + String(drawerId));
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, medicationName);
    printLine(1, String("Drawer ") + String(drawerId));
}

void LCDScreen::showMessage(const String& message) {
    lastMessage = message;
    printSerialScreen(message);
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, message);
}

void LCDScreen::showDoseConfirmed() {
    lastMessage = "Dose confirmed";
    printSerialScreen("Dose confirmed", "Thank you");
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, "Dose confirmed");
    printLine(1, "Thank you");
}

void LCDScreen::showDoseMissed() {
    lastMessage = "Dose missed";
    printSerialScreen("Dose missed", "Check drawer");
    if (!initialized) {
        return;
    }

    lcd.clear();
    printLine(0, "Dose missed");
    printLine(1, "Check drawer");
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
