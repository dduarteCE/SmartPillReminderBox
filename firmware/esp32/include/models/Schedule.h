#pragma once

#include <Arduino.h>
#include "config/SystemConfig.h"
#include "hardware/ClockModule.h"

struct ScheduleTime {
    int hour;
    int minute;
};

class Schedule {
public:
    Schedule();
    Schedule(int id, int drawerId);

    int getId() const;
    int getDrawerId() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool addTime(int hour, int minute);
    bool addDayOfWeek(const String& dayOfWeek);
    bool containsTime(int hour, int minute) const;
    bool containsDay(const String& dayOfWeek) const;
    bool matchesCurrentTime(const DateTime& currentDateTime) const;
    bool shouldTrigger(const DateTime& currentDateTime) const;

private:
    int id;
    int drawerId;
    bool enabled;
    ScheduleTime times[MAX_SCHEDULE_TIMES];
    int timeCount;
    String daysOfWeek[MAX_DAYS_OF_WEEK];
    int dayCount;
};
