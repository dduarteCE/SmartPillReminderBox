#pragma once

#include <Arduino.h>
#include "config/SystemConfig.h"
#include "hardware/ClockModule.h"
#include "models/DoseEvent.h"
#include "models/Schedule.h"

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
    void setSchedules(const Schedule schedules[], int count);
    bool addSchedule(const Schedule& schedule);
    bool updateSchedule(const Schedule& schedule);
    bool removeSchedule(int scheduleId);
    bool deleteSchedule(int scheduleId);
    void checkSchedules(const DateTime& currentDateTime);
    void startReminder(const Schedule& schedule, const ScheduleTime& scheduledTime);
    void updateReminderState();
    void update();
    void markDrawerOpened();
    void markDrawerClosed();
    void markDoseCompleted();
    void markDoseMissed();
    void resetCurrentReminder();
    ReminderState getState() const;
    bool hasPendingEvent() const;
    DoseEvent popPendingEvent();

private:
    bool pushPendingEvent(const DoseEvent& event);

    Schedule schedules[MAX_SCHEDULES];
    int scheduleCount;
    DoseEvent pendingEvents[MAX_PENDING_EVENTS];
    int pendingEventCount;
    Schedule currentSchedule;
    ScheduleTime currentScheduledTime;
    ReminderState state;
    unsigned long reminderStartTime;
    unsigned long interactionWindowMs;
    bool hasDrawerOpened;
};
