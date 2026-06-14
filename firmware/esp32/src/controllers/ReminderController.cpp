#include "controllers/ReminderController.h"

ReminderController::ReminderController()
    : scheduleCount(0),
      pendingEventCount(0),
      currentSchedule(),
      currentScheduledTime({0, 0}),
      state(ReminderState::Idle),
      reminderStartTime(0),
      interactionWindowMs(REMINDER_INTERACTION_WINDOW_MS),
      hasDrawerOpened(false) {}

void ReminderController::begin() {}

void ReminderController::setSchedules(const Schedule schedules[], int count) {
    scheduleCount = min(count, MAX_SCHEDULES);
    for (int index = 0; index < scheduleCount; index++) {
        this->schedules[index] = schedules[index];
    }
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
            startReminder(schedules[index], {currentDateTime.hour, currentDateTime.minute});
            return;
        }
    }
}

void ReminderController::startReminder(const Schedule& schedule, const ScheduleTime& scheduledTime) {
    currentSchedule = schedule;
    currentScheduledTime = scheduledTime;
    reminderStartTime = millis();
    hasDrawerOpened = false;
    state = ReminderState::ReminderActive;
}

void ReminderController::updateReminderState() {
    if (state == ReminderState::ReminderActive || state == ReminderState::WaitingForClose) {
        if (millis() - reminderStartTime >= interactionWindowMs) {
            markDoseMissed();
        }
    }
}

void ReminderController::update() {
    updateReminderState();
}

void ReminderController::markDrawerOpened() {
    hasDrawerOpened = true;
    state = ReminderState::WaitingForClose;
}

void ReminderController::markDrawerClosed() {
    if (hasDrawerOpened) {
        markDoseCompleted();
    }
}

void ReminderController::markDoseCompleted() {
    state = ReminderState::InteractionComplete;
}

void ReminderController::markDoseMissed() {
    state = ReminderState::Timeout;
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
