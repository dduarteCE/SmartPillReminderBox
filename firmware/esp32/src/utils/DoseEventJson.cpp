#include "utils/DoseEventJson.h"

String doseEventTypeToString(DoseEventType type) {
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

DoseEventType doseEventTypeFromString(const String& type) {
    if (type == "DRAWER_OPENED") {
        return DoseEventType::DrawerOpened;
    }
    if (type == "DRAWER_CLOSED") {
        return DoseEventType::DrawerClosed;
    }
    if (type == "DOSE_COMPLETED") {
        return DoseEventType::DoseCompleted;
    }
    if (type == "DOSE_MISSED") {
        return DoseEventType::DoseMissed;
    }

    return DoseEventType::ReminderStarted;
}

String doseEventStatusToString(DoseEventStatus status) {
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

DoseEventStatus doseEventStatusFromString(const String& status) {
    if (status == "TAKEN") {
        return DoseEventStatus::Taken;
    }
    if (status == "MISSED") {
        return DoseEventStatus::Missed;
    }

    return DoseEventStatus::Pending;
}
