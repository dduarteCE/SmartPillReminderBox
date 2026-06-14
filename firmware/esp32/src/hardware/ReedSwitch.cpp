#include "hardware/ReedSwitch.h"

ReedSwitch::ReedSwitch(int pin) : pin(pin), lastState(false) {}

void ReedSwitch::begin() {
    if (pin < 0) {
        return;
    }

    pinMode(pin, INPUT_PULLUP);
    lastState = readState();
}

bool ReedSwitch::isOpen() {
    return readState();
}

bool ReedSwitch::isClosed() {
    return !readState();
}

bool ReedSwitch::readState() {
    if (pin < 0) {
        return false;
    }

    lastState = digitalRead(pin) == HIGH;
    return lastState;
}
