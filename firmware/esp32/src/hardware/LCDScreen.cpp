#include "hardware/LCDScreen.h"

LCDScreen::LCDScreen(uint8_t address, uint8_t columns, uint8_t rows)
    : address(address), columns(columns), rows(rows), lastMessage("") {}

void LCDScreen::begin() {
    clear();
}

void LCDScreen::clear() {
    lastMessage = "";
}

void LCDScreen::showReminder(const String& medicationName, int drawerId) {
    lastMessage = String("Take ") + medicationName + " D" + String(drawerId);
}

void LCDScreen::showMessage(const String& message) {
    lastMessage = message;
}

void LCDScreen::showDoseConfirmed() {
    lastMessage = "Dose confirmed";
}

void LCDScreen::showDoseMissed() {
    lastMessage = "Dose missed";
}
