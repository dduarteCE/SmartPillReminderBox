#include "hardware/ClockModule.h"

ClockModule::ClockModule() : currentDateTime({"", "00:00", "", 0, 0}) {}

void ClockModule::begin() {}

void ClockModule::setTime(const String& currentDate, const String& currentTime, const String& dayOfWeek) {
    currentDateTime.date = currentDate;
    currentDateTime.time = currentTime;
    currentDateTime.dayOfWeek = dayOfWeek;
    currentDateTime.hour = currentTime.substring(0, 2).toInt();
    currentDateTime.minute = currentTime.substring(3, 5).toInt();
}

DateTime ClockModule::getCurrentDateTime() const {
    return currentDateTime;
}

String ClockModule::getDayOfWeek() const {
    return currentDateTime.dayOfWeek;
}
