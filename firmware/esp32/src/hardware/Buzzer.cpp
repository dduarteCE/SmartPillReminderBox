#include "hardware/Buzzer.h"

Buzzer::Buzzer(int pin) : pin(pin), active(false) {}

void Buzzer::begin() {
    if (pin < 0) {
        return;
    }

    pinMode(pin, OUTPUT);
    deactivate();
}

void Buzzer::activate() {
    active = true;
    if (pin >= 0) {
        digitalWrite(pin, HIGH);
    }
}

void Buzzer::deactivate() {
    active = false;
    if (pin >= 0) {
        digitalWrite(pin, LOW);
    }
}

bool Buzzer::isActive() const {
    return active;
}
