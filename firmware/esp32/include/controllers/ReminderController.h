#pragma once

#include <Arduino.h>
#include "config/SystemConfig.h"
#include "hardware/ClockModule.h"
#include "models/DoseEvent.h"
#include "models/Schedule.h"

class DrawerManager;

enum class ReminderState {
    Idle,
    ReminderActive,
    DrawerOpen,
    WaitingForClose,
    Timeout,
    InteractionComplete,
    SendingFeedback
};

class ReminderController {
public:
    ReminderController();

    void begin();
    void setDrawerManager(DrawerManager* drawerManager);
    void setSchedules(const Schedule schedules[], int count);
    bool addSchedule(const Schedule& schedule);
    bool updateSchedule(const Schedule& schedule);
    bool removeSchedule(int scheduleId);
    bool deleteSchedule(int scheduleId);
    void checkSchedules(const DateTime& currentDateTime);
    void startReminder(const Schedule& schedule, const DateTime& currentDateTime);
    void updateReminderState(const DateTime& currentDateTime);
    void update(const DateTime& currentDateTime);
    void markDrawerOpened(const DateTime& currentDateTime);
    void markDrawerClosed(const DateTime& currentDateTime);
    void markDoseCompleted(const DateTime& currentDateTime);
    void markDoseMissed(const DateTime& currentDateTime);
    void resetCurrentReminder();
    ReminderState getState() const;
    bool hasPendingEvent() const;
    DoseEvent popPendingEvent();

private:
    DoseEvent buildEvent(
        DoseEventType type,
        DoseEventStatus status,
        const DateTime& currentDateTime
    );
    bool pushPendingEvent(const DoseEvent& event);

    DrawerManager* drawerManager;
    Schedule schedules[MAX_SCHEDULES];
    int scheduleCount;
    DoseEvent pendingEvents[MAX_PENDING_EVENTS];
    int pendingEventCount;
    Schedule currentSchedule;
    ScheduleTime currentScheduledTime;
    ReminderState state;
    unsigned long reminderStartTime;
    int nextEventID;
    int lastTriggeredScheduleId;
    int lastTriggeredHour;
    int lastTriggeredMinute;
    String lastTriggeredDate;
    unsigned long interactionWindowMs;
    bool hasDrawerOpened;
};
