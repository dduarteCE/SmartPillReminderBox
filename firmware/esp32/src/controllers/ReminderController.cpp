#include "controllers/ReminderController.h"
#include "controllers/DrawerManager.h"

ReminderController::ReminderController()
    : drawerManager(nullptr),
      scheduleCount(0),
      pendingEventCount(0),
      currentSchedule(),
      currentScheduledTime({0, 0}),
      state(ReminderState::Idle),
      reminderStartTime(0),
      nextEventID(1),
      lastTriggeredScheduleId(0),
      lastTriggeredHour(-1),
      lastTriggeredMinute(-1),
      lastTriggeredDate(""),
      interactionWindowMs(REMINDER_INTERACTION_WINDOW_MS),
      hasDrawerOpened(false) {}

void ReminderController::begin() {}

void ReminderController::setDrawerManager(DrawerManager* drawerManager) {
    this->drawerManager = drawerManager;
}

void ReminderController::setSchedules(const Schedule schedules[], int count) {
    scheduleCount = min(count, MAX_SCHEDULES);
    for (int index = 0; index < scheduleCount; index++) {
        this->schedules[index] = schedules[index];
    }
}

int ReminderController::getSchedules(Schedule outputSchedules[], int maxSchedules) const {
    int count = min(scheduleCount, maxSchedules);
    for (int index = 0; index < count; index++) {
        outputSchedules[index] = schedules[index];
    }

    return count;
}

int ReminderController::getScheduleCount() const {
    return scheduleCount;
}

bool ReminderController::addSchedule(const Schedule& schedule) {
    if (scheduleCount >= MAX_SCHEDULES) {
        return false;
    }

    schedules[scheduleCount++] = schedule;
    return true;
}

bool ReminderController::updateSchedule(const Schedule& schedule) {
    for (int index = 0; index < scheduleCount; index++) {
        if (schedules[index].getId() == schedule.getId()) {
            schedules[index] = schedule;
            return true;
        }
    }

    return false;
}

bool ReminderController::removeSchedule(int scheduleId) {
    return deleteSchedule(scheduleId);
}

bool ReminderController::deleteSchedule(int scheduleId) {
    for (int index = 0; index < scheduleCount; index++) {
        if (schedules[index].getId() == scheduleId) {
            for (int shift = index; shift < scheduleCount - 1; shift++) {
                schedules[shift] = schedules[shift + 1];
            }
            scheduleCount--;
            return true;
        }
    }

    return false;
}

void ReminderController::checkSchedules(const DateTime& currentDateTime) {
    if (state != ReminderState::Idle) {
        return;
    }

    for (int index = 0; index < scheduleCount; index++) {
        if (schedules[index].shouldTrigger(currentDateTime)) {
            bool alreadyTriggered =
                schedules[index].getId() == lastTriggeredScheduleId &&
                currentDateTime.hour == lastTriggeredHour &&
                currentDateTime.minute == lastTriggeredMinute &&
                currentDateTime.date == lastTriggeredDate;

            if (alreadyTriggered) {
                continue;
            }

            lastTriggeredScheduleId = schedules[index].getId();
            lastTriggeredHour = currentDateTime.hour;
            lastTriggeredMinute = currentDateTime.minute;
            lastTriggeredDate = currentDateTime.date;
            startReminder(schedules[index], currentDateTime);
            return;
        }
    }
}

void ReminderController::startReminder(const Schedule& schedule, const DateTime& currentDateTime) {
    if (state != ReminderState::Idle) {
        return;
    }

    currentSchedule = schedule;
    currentScheduledTime = {currentDateTime.hour, currentDateTime.minute};
    reminderStartTime = millis();
    hasDrawerOpened = false;
    state = ReminderState::ReminderActive;
    DoseEvent event = buildEvent(
        DoseEventType::ReminderStarted,
        DoseEventStatus::Pending,
        currentDateTime
    );
    pushPendingEvent(event);
    nextEventID++;
}

void ReminderController::updateReminderState(const DateTime& currentDateTime) {
    if (state != ReminderState::ReminderActive && state != ReminderState::WaitingForClose) {
        return;
    }

    if (drawerManager == nullptr) {
        return;
    }

    bool drawerOpen = drawerManager->isDrawerOpen(currentSchedule.getDrawerId());

    if (state == ReminderState::ReminderActive && drawerOpen) {
        markDrawerOpened(currentDateTime);
        return;
    }

    if (state == ReminderState::WaitingForClose && !drawerOpen) {
        markDrawerClosed(currentDateTime);
        return;
    }


    if (millis() - reminderStartTime >= interactionWindowMs) {
        markDoseMissed(currentDateTime);
        return;
    }
}

void ReminderController::update(const DateTime& currentDateTime) {
    updateReminderState(currentDateTime);
}

void ReminderController::markDrawerOpened(const DateTime& currentDateTime) {
    if (state != ReminderState::ReminderActive) {
        return;
    }
    hasDrawerOpened = true;
    state = ReminderState::WaitingForClose;
    DoseEvent event = buildEvent(
        DoseEventType::DrawerOpened,
        DoseEventStatus::Pending,
        currentDateTime
    );
    pushPendingEvent(event);
    nextEventID++;
}

void ReminderController::markDrawerClosed(const DateTime& currentDateTime) {
    if (state != ReminderState::WaitingForClose || !hasDrawerOpened) {
        return;
    }

    DoseEvent event = buildEvent(
        DoseEventType::DrawerClosed,
        DoseEventStatus::Pending,
        currentDateTime
    );

    pushPendingEvent(event);
    nextEventID++;
    markDoseCompleted(currentDateTime);
}

void ReminderController::markDoseCompleted(const DateTime& currentDateTime) {
    if (state != ReminderState::WaitingForClose) {
        return;
    }

    state = ReminderState::InteractionComplete;
    DoseEvent event = buildEvent(
        DoseEventType::DoseCompleted,
        DoseEventStatus::Taken,
        currentDateTime
    );
    pushPendingEvent(event);
    nextEventID++;
    resetCurrentReminder();
}

void ReminderController::markDoseMissed(const DateTime& currentDateTime) {
    if (state != ReminderState::ReminderActive && state != ReminderState::WaitingForClose) {
        return;
    }
    state = ReminderState::Timeout;
    DoseEvent event = buildEvent(
        DoseEventType::DoseMissed,
        DoseEventStatus::Missed,
        currentDateTime
    );
    pushPendingEvent(event);
    nextEventID++;
    resetCurrentReminder();
}

void ReminderController::resetCurrentReminder() {
    currentSchedule = Schedule();
    currentScheduledTime = {0, 0};
    hasDrawerOpened = false;
    state = ReminderState::Idle;
}

ReminderState ReminderController::getState() const {
    return state;
}

void ReminderController::setNextEventId(int nextEventId) {
    if (nextEventId > nextEventID) {
        nextEventID = nextEventId;
    }
}

DoseEvent ReminderController::buildEvent(
    DoseEventType type,
    DoseEventStatus status,
    const DateTime& currentDateTime
) {
    Drawer* drawer = drawerManager != nullptr
        ? drawerManager->getDrawer(currentSchedule.getDrawerId())
        : nullptr;

    String medicationName = drawer != nullptr ? drawer->getMedicationName() : "";
    char scheduledTimeBuffer[6];
    snprintf(
        scheduledTimeBuffer,
        sizeof(scheduledTimeBuffer),
        "%02d:%02d",
        currentScheduledTime.hour,
        currentScheduledTime.minute
    );
    String scheduledTimeString = String(scheduledTimeBuffer);
    String timestampString = currentDateTime.date + "T" + currentDateTime.time;

    return DoseEvent(
        nextEventID,
        type,
        currentSchedule.getId(),
        scheduledTimeString,
        currentSchedule.getDrawerId(),
        medicationName,
        timestampString,
        status
    );
}

bool ReminderController::hasPendingEvent() const {
    return pendingEventCount > 0;
}

DoseEvent ReminderController::popPendingEvent() {
    if (pendingEventCount == 0) {
        return DoseEvent();
    }

    DoseEvent event = pendingEvents[0];
    for (int index = 0; index < pendingEventCount - 1; index++) {
        pendingEvents[index] = pendingEvents[index + 1];
    }
    pendingEventCount--;
    return event;
}

bool ReminderController::pushPendingEvent(const DoseEvent& event) {
    if (pendingEventCount >= MAX_PENDING_EVENTS) {
        return false;
    }

    pendingEvents[pendingEventCount++] = event;
    return true;
}
