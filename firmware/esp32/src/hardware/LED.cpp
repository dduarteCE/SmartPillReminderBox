#include "hardware/LED.h"

LED::LED(int pin)
    : pin(pin),
      active(false),
      blinking(false),
      blinkIntervalMs(500),
      lastToggleTime(0) {}

void LED::begin() {
    if (pin < 0) {
        return;
    }

    pinMode(pin, OUTPUT);
    turnOff();
}

void LED::turnOn() {
    blinking = false;
    active = true;
    if (pin >= 0) {
        digitalWrite(pin, HIGH);
    }
}

void LED::turnOff() {
    blinking = false;
    active = false;
    if (pin >= 0) {
        digitalWrite(pin, LOW);
    }
}

void LED::toggle() {
    active = !active;
    if (pin >= 0) {
        digitalWrite(pin, active ? HIGH : LOW);
    }
}

void LED::blink(unsigned long intervalMs) {
    blinking = true;
    blinkIntervalMs = intervalMs;
}

void LED::update() {
    if (!blinking) {
        return;
    }

    unsigned long now = millis();
    if (now - lastToggleTime >= blinkIntervalMs) {
        lastToggleTime = now;
        toggle();
    }
}

bool LED::isOn() const {
    return active;
}
