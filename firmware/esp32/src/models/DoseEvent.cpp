#include "models/DoseEvent.h"

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

String DoseEvent::toJson() const {
    String json = "{";
    json += "\"id\":";
    json += String(id);
    json += ",\"type\":\"";
    json += typeToString();
    json += "\",\"scheduleId\":";
    json += String(scheduleId);
    json += ",\"scheduledTime\":\"";
    json += scheduledTime;
    json += "\",\"drawerId\":";
    json += String(drawerId);
    json += ",\"medicationName\":\"";
    json += medicationName;
    json += "\",\"timestamp\":\"";
    json += timestamp;
    json += "\",\"status\":\"";
    json += statusToString();
    json += "\"}";
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
