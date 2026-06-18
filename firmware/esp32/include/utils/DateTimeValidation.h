#pragma once

#include <Arduino.h>

bool isDigits(const String& value);
bool isValidTimeString(const String& currentTime);
bool isValidDateString(const String& currentDate);
bool isValidDayOfWeek(const String& dayOfWeek);
