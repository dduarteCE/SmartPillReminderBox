#pragma once

#include <Arduino.h>

struct DateTime {
    String date;
    String time;
    String dayOfWeek;
    int hour;
    int minute;
};

class ClockModule {
public:
    ClockModule();

    void begin();
    void setTime(const String& currentDate, const String& currentTime, const String& dayOfWeek);
    DateTime getCurrentDateTime() const;
    String getDayOfWeek() const;
    bool isConfigured() const;

private:
    DateTime baseDateTime;
    unsigned long baseMillis;
    bool configured;
};
