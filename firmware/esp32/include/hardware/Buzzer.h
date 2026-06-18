#pragma once

#include <Arduino.h>

class Buzzer {
public:
    explicit Buzzer(int pin = -1);

    void begin();
    void activate();
    void deactivate();
    bool isActive() const;

private:
    int pin;
    bool active;
};
