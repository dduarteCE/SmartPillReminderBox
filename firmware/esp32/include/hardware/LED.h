#pragma once

#include <Arduino.h>

class LED {
public:
    explicit LED(int pin = -1);

    void begin();
    void turnOn();
    void turnOff();
    void toggle();
    void blink(unsigned long intervalMs = 500);
    void update();
    bool isOn() const;

private:
    int pin;
    bool active;
    bool blinking;
    unsigned long blinkIntervalMs;
    unsigned long lastToggleTime;
};
