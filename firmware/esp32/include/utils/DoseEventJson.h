#pragma once

#include <Arduino.h>

#include "models/DoseEvent.h"

String doseEventTypeToString(DoseEventType type);
DoseEventType doseEventTypeFromString(const String& type);
String doseEventStatusToString(DoseEventStatus status);
DoseEventStatus doseEventStatusFromString(const String& status);
