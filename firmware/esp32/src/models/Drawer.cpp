#include "models/Drawer.h"

Drawer::Drawer() : id(0), medicationName(""), enabled(false), led(), reedSwitch() {}

Drawer::Drawer(int id, const String& medicationName, bool enabled, int ledPin, int reedSwitchPin)
    : id(id), medicationName(medicationName), enabled(enabled), led(ledPin), reedSwitch(reedSwitchPin) {}

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

bool Drawer::isOpen() {
    return reedSwitch.isOpen();
}

void Drawer::setMedicationName(const String& medicationName) {
    this->medicationName = medicationName;
}

void Drawer::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void Drawer::highlight() {
    led.blink();
}

void Drawer::stopHighlight() {
    led.turnOff();
}
