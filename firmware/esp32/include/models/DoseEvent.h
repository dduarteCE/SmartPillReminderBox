#pragma once

#include <Arduino.h>

enum class DoseEventType {
    ReminderStarted,
    DrawerOpened,
    DrawerClosed,
    DoseCompleted,
    DoseMissed,
    DrawerEmpty
};

enum class DoseEventStatus {
    Pending,
    Taken,
    Missed
};

class DoseEvent {
public:
    DoseEvent();
    DoseEvent(
        int id,
        DoseEventType type,
        int scheduleId,
        const String& scheduledTime,
        int drawerId,
        const String& medicationName,
        const String& timestamp,
        DoseEventStatus status
    );

    int getId() const;
    DoseEventType getType() const;
    int getScheduleId() const;
    String getScheduledTime() const;
    int getDrawerId() const;
    String getMedicationName() const;
    String getTimestamp() const;
    DoseEventStatus getStatus() const;
    String toJson() const;

private:
    String typeToString() const;
    String statusToString() const;

    int id;
    DoseEventType type;
    int scheduleId;
    String scheduledTime;
    int drawerId;
    String medicationName;
    String timestamp;
    DoseEventStatus status;
};
