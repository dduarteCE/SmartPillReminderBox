#include "models/DoseEvent.h"

#include <ArduinoJson.h>

DoseEvent::DoseEvent()
    : id(0),
      type(DoseEventType::ReminderStarted),
      scheduleId(0),
      scheduledTime(""),
      drawerId(0),
      medicationName(""),
      timestamp(""),
      status(DoseEventStatus::Pending) {}

DoseEvent::DoseEvent(
    int id,
    DoseEventType type,
    int scheduleId,
    const String& scheduledTime,
    int drawerId,
    const String& medicationName,
    const String& timestamp,
    DoseEventStatus status
) : id(id),
    type(type),
    scheduleId(scheduleId),
    scheduledTime(scheduledTime),
    drawerId(drawerId),
    medicationName(medicationName),
    timestamp(timestamp),
    status(status) {}

int DoseEvent::getId() const {
    return id;
}

DoseEventType DoseEvent::getType() const {
    return type;
}

int DoseEvent::getScheduleId() const {
    return scheduleId;
}

String DoseEvent::getScheduledTime() const {
    return scheduledTime;
}

int DoseEvent::getDrawerId() const {
    return drawerId;
}

String DoseEvent::getMedicationName() const {
    return medicationName;
}

String DoseEvent::getTimestamp() const {
    return timestamp;
}

DoseEventStatus DoseEvent::getStatus() const {
    return status;
}

String DoseEvent::toJson() const {
    JsonDocument doc;
    doc["id"] = id;
    doc["type"] = typeToString();
    doc["scheduleId"] = scheduleId;
    doc["scheduledTime"] = scheduledTime;
    doc["drawerId"] = drawerId;
    doc["medicationName"] = medicationName;
    doc["timestamp"] = timestamp;
    doc["status"] = statusToString();

    String json;
    serializeJson(doc, json);
    return json;
}

String DoseEvent::typeToString() const {
    switch (type) {
        case DoseEventType::DrawerOpened:
            return "DRAWER_OPENED";
        case DoseEventType::DrawerClosed:
            return "DRAWER_CLOSED";
        case DoseEventType::DoseCompleted:
            return "DOSE_COMPLETED";
        case DoseEventType::DoseMissed:
            return "DOSE_MISSED";
        case DoseEventType::ReminderStarted:
        default:
            return "REMINDER_STARTED";
    }
}

String DoseEvent::statusToString() const {
    switch (status) {
        case DoseEventStatus::Taken:
            return "TAKEN";
        case DoseEventStatus::Missed:
            return "MISSED";
        case DoseEventStatus::Pending:
        default:
            return "PENDING";
    }
}
