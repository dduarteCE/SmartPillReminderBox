#include "models/Drawer.h"

Drawer::Drawer() : id(0), medicationName(""), enabled(false), pillCount(0), led(), reedSwitch() {}

Drawer::Drawer(
    int id,
    const String& medicationName,
    bool enabled,
    int ledPin,
    int reedSwitchPin,
    int pillCount
) : id(id),
    medicationName(medicationName),
    enabled(enabled),
    pillCount(max(0, pillCount)),
    led(ledPin),
    reedSwitch(reedSwitchPin) {}

void Drawer::begin() {
    led.begin();
    reedSwitch.begin();
}

void Drawer::update() {
    led.update();
}

int Drawer::getId() const {
    return id;
}

String Drawer::getMedicationName() const {
    return medicationName;
}

bool Drawer::isEnabled() const {
    return enabled;
}

int Drawer::getPillCount() const {
    return pillCount;
}

bool Drawer::isOpen() {
    return reedSwitch.isOpen();
}

void Drawer::setMedicationName(const String& medicationName) {
    this->medicationName = medicationName;
}

void Drawer::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void Drawer::setPillCount(int pillCount) {
    this->pillCount = max(0, pillCount);
}

bool Drawer::recordDoseTaken() {
    if (pillCount <= 0) {
        return false;
    }

    pillCount--;
    return pillCount == 0;
}

void Drawer::highlight() {
    led.blink();
}

void Drawer::stopHighlight() {
    led.turnOff();
}
