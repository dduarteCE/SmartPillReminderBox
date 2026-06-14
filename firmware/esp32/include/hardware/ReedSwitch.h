#pragma once

#include <Arduino.h>

class ReedSwitch {
public:
    explicit ReedSwitch(int pin = -1);

    void begin();
    bool isOpen();
    bool isClosed();
    bool readState();

private:
    int pin;
    bool lastState;
};
