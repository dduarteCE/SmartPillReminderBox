#include "hardware/ClockModule.h"

namespace {

constexpr unsigned long MINUTES_PER_DAY = 24UL * 60UL;

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int daysInMonth(int year, int month) {
    switch (month) {
        case 2:
            return isLeapYear(year) ? 29 : 28;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            return 31;
    }
}

int parseDatePart(const String& date, int start, int length) {
    return date.substring(start, start + length).toInt();
}

String formatDate(int year, int month, int day) {
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
    return String(buffer);
}

String formatTime(int hour, int minute) {
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    return String(buffer);
}

String nextDayOfWeek(const String& dayOfWeek) {
    static const char* days[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    for (int index = 0; index < 7; index++) {
        if (dayOfWeek == days[index]) {
            return String(days[(index + 1) % 7]);
        }
    }

    return dayOfWeek;
}

DateTime advanceDateTime(const DateTime& baseDateTime, uint64_t elapsedMinutes) {
    DateTime current = baseDateTime;
    uint64_t totalMinutes = static_cast<uint64_t>(baseDateTime.hour) * 60ULL
        + static_cast<uint64_t>(baseDateTime.minute)
        + elapsedMinutes;
    uint64_t elapsedDays = totalMinutes / MINUTES_PER_DAY;

    current.hour = (totalMinutes % MINUTES_PER_DAY) / 60UL;
    current.minute = totalMinutes % 60UL;
    current.time = formatTime(current.hour, current.minute);

    if (current.date.length() != 10) {
        return current;
    }

    int year = parseDatePart(current.date, 0, 4);
    int month = parseDatePart(current.date, 5, 2);
    int day = parseDatePart(current.date, 8, 2);

    for (uint64_t index = 0; index < elapsedDays; index++) {
        day++;
        if (day > daysInMonth(year, month)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
        current.dayOfWeek = nextDayOfWeek(current.dayOfWeek);
    }

    current.date = formatDate(year, month, day);
    return current;
}

}

ClockModule::ClockModule()
    : baseDateTime({"", "00:00", "", 0, 0}),
      lastMillis(0),
      accumulatedMillis(0),
      configured(false) {}

void ClockModule::begin() {}

void ClockModule::setTime(const String& currentDate, const String& currentTime, const String& dayOfWeek) {
    baseDateTime.date = currentDate;
    baseDateTime.time = currentTime;
    baseDateTime.dayOfWeek = dayOfWeek;
    baseDateTime.hour = currentTime.substring(0, 2).toInt();
    baseDateTime.minute = currentTime.substring(3, 5).toInt();
    lastMillis = millis();
    accumulatedMillis = 0;
    configured = true;
}

DateTime ClockModule::getCurrentDateTime() const {
    if (!configured) {
        return baseDateTime;
    }

    unsigned long now = millis();
    accumulatedMillis += static_cast<unsigned long>(now - lastMillis);
    lastMillis = now;

    uint64_t elapsedMinutes = accumulatedMillis / 60000ULL;
    return advanceDateTime(baseDateTime, elapsedMinutes);
}

String ClockModule::getDayOfWeek() const {
    return getCurrentDateTime().dayOfWeek;
}

bool ClockModule::isConfigured() const {
    return configured;
}
