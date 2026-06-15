#include "utils/DateTimeValidation.h"

bool isDigits(const String& value) {
    for (int index = 0; index < value.length(); index++) {
        if (!isDigit(value[index])) {
            return false;
        }
    }

    return value.length() > 0;
}

bool isValidTimeString(const String& currentTime) {
    if (currentTime.length() != 5 || currentTime[2] != ':') {
        return false;
    }

    String hourString = currentTime.substring(0, 2);
    String minuteString = currentTime.substring(3, 5);
    if (!isDigits(hourString) || !isDigits(minuteString)) {
        return false;
    }

    int hour = hourString.toInt();
    int minute = minuteString.toInt();
    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

namespace {

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

}

bool isValidDateString(const String& currentDate) {
    if (currentDate.length() != 10 || currentDate[4] != '-' || currentDate[7] != '-') {
        return false;
    }

    String yearString = currentDate.substring(0, 4);
    String monthString = currentDate.substring(5, 7);
    String dayString = currentDate.substring(8, 10);

    if (!isDigits(yearString) || !isDigits(monthString) || !isDigits(dayString)) {
        return false;
    }

    int year = yearString.toInt();
    int month = monthString.toInt();
    int day = dayString.toInt();

    if (month < 1 || month > 12 || day < 1) {
        return false;
    }

    return day <= daysInMonth(year, month);
}
