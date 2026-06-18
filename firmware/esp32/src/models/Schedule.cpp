#include "models/Schedule.h"

Schedule::Schedule() : id(0), drawerId(0), enabled(false), timeCount(0), dayCount(0) {}

Schedule::Schedule(int id, int drawerId)
    : id(id), drawerId(drawerId), enabled(true), timeCount(0), dayCount(0) {}

int Schedule::getId() const {
    return id;
}

int Schedule::getDrawerId() const {
    return drawerId;
}

bool Schedule::isEnabled() const {
    return enabled;
}

void Schedule::setEnabled(bool enabled) {
    this->enabled = enabled;
}

int Schedule::getTimeCount() const {
    return timeCount;
}

ScheduleTime Schedule::getTime(int index) const {
    if (index < 0 || index >= timeCount) {
        return {-1, -1};
    }

    return times[index];
}

int Schedule::getDayCount() const {
    return dayCount;
}

String Schedule::getDayOfWeek(int index) const {
    if (index < 0 || index >= dayCount) {
        return "";
    }

    return daysOfWeek[index];
}

bool Schedule::addTime(int hour, int minute) {
    if (timeCount >= MAX_SCHEDULE_TIMES) {
        return false;
    }

    times[timeCount++] = {hour, minute};
    return true;
}

bool Schedule::addDayOfWeek(const String& dayOfWeek) {
    if (dayCount >= MAX_DAYS_OF_WEEK) {
        return false;
    }

    daysOfWeek[dayCount++] = dayOfWeek;
    return true;
}

bool Schedule::containsTime(int hour, int minute) const {
    for (int index = 0; index < timeCount; index++) {
        if (times[index].hour == hour && times[index].minute == minute) {
            return true;
        }
    }

    return false;
}

bool Schedule::containsDay(const String& dayOfWeek) const {
    if (dayCount == 0) {
        return true;
    }

    for (int index = 0; index < dayCount; index++) {
        if (daysOfWeek[index] == dayOfWeek) {
            return true;
        }
    }

    return false;
}

bool Schedule::matchesCurrentTime(const DateTime& currentDateTime) const {
    return shouldTrigger(currentDateTime);
}

bool Schedule::shouldTrigger(const DateTime& currentDateTime) const {
    return enabled
        && containsDay(currentDateTime.dayOfWeek)
        && containsTime(currentDateTime.hour, currentDateTime.minute);
}
