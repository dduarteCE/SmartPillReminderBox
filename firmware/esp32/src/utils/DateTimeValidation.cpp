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

bool isValidDateString(const String& currentDate) {
    if (currentDate.length() != 10 || currentDate[4] != '-' || currentDate[7] != '-') {
        return false;
    }

    return isDigits(currentDate.substring(0, 4))
        && isDigits(currentDate.substring(5, 7))
        && isDigits(currentDate.substring(8, 10));
}
