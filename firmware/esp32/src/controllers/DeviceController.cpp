#include "controllers/DeviceController.h"

#include "config/PinConfig.h"

DeviceController::DeviceController()
    : reminderController(),
      drawerManager(),
      webServerController(this),
      webSocketService(),
      storageManager(),
      lcdScreen(),
      buzzer(BUZZER_PIN),
      clockModule(),
      unacknowledgedEventCount(0),
      storageReady(false) {}

void DeviceController::setup() {
    storageReady = storageManager.begin();
    loadStoredConfiguration();
    drawerManager.begin();
    reminderController.setDrawerManager(&drawerManager);
    reminderController.begin();
    lcdScreen.begin();
    buzzer.begin();
    clockModule.begin();
    webServerController.begin();
    webSocketService.begin();
}

void DeviceController::loop() {
    webServerController.handleClient();
    webSocketService.handleWebSocket();
    drawerManager.update();
    updateReminderFeedback();

    DateTime currentDateTime = clockModule.getCurrentDateTime();

    reminderController.checkSchedules(currentDateTime);
    reminderController.update(currentDateTime);

    while (reminderController.hasPendingEvent()) {
        publishDoseEvent(reminderController.popPendingEvent());
    }
}

bool DeviceController::isReady() const {
    return storageReady;
}

bool DeviceController::isStorageReady() const {
    return storageReady;
}

bool DeviceController::isWifiConnected() const {
    return webServerController.isWifiReady();
}

bool DeviceController::isWebSocketEnabled() const {
    return webSocketService.isEnabled();
}

int DeviceController::getDrawers(Drawer outputDrawers[], int maxDrawers) const {
    return drawerManager.getDrawers(outputDrawers, maxDrawers);
}

bool DeviceController::getDrawer(int drawerId, Drawer& drawer) const {
    const Drawer* foundDrawer = drawerManager.getDrawer(drawerId);
    if (foundDrawer == nullptr) {
        return false;
    }

    drawer = *foundDrawer;
    return true;
}

int DeviceController::getSchedules(Schedule outputSchedules[], int maxSchedules) const {
    return reminderController.getSchedules(outputSchedules, maxSchedules);
}

bool DeviceController::getSchedule(int scheduleId, Schedule& schedule) const {
    Schedule schedules[MAX_SCHEDULES];
    int count = reminderController.getSchedules(schedules, MAX_SCHEDULES);
    for (int index = 0; index < count; index++) {
        if (schedules[index].getId() == scheduleId) {
            schedule = schedules[index];
            return true;
        }
    }

    return false;
}

int DeviceController::getNextScheduleId() const {
    Schedule schedules[MAX_SCHEDULES];
    int count = reminderController.getSchedules(schedules, MAX_SCHEDULES);
    int nextScheduleId = 1;
    for (int index = 0; index < count; index++) {
        if (schedules[index].getId() >= nextScheduleId) {
            nextScheduleId = schedules[index].getId() + 1;
        }
    }

    return nextScheduleId;
}

int DeviceController::getUnacknowledgedEvents(DoseEvent outputEvents[], int maxEvents) const {
    int count = min(unacknowledgedEventCount, maxEvents);
    for (int index = 0; index < count; index++) {
        outputEvents[index] = unacknowledgedEvents[index];
    }

    return count;
}

DateTime DeviceController::getCurrentDateTime() const {
    return clockModule.getCurrentDateTime();
}

bool DeviceController::setCurrentDateTime(
    const String& currentDate,
    const String& currentTime,
    const String& dayOfWeek
) {
    clockModule.setTime(currentDate, currentTime, dayOfWeek);
    return true;
}

bool DeviceController::applyDrawerConfig(
    int drawerId,
    const String& medicationName,
    bool enabled,
    int pillCount
) {
    Drawer previousDrawer;
    bool hadPreviousDrawer = getDrawer(drawerId, previousDrawer);
    if (!drawerManager.configureDrawer(drawerId, medicationName, enabled, pillCount)) {
        return false;
    }

    if (persistDrawers()) {
        return true;
    }

    if (hadPreviousDrawer) {
        drawerManager.configureDrawer(
            drawerId,
            previousDrawer.getMedicationName(),
            previousDrawer.isEnabled(),
            previousDrawer.getPillCount()
        );
    }
    return false;
}

bool DeviceController::deleteDrawerConfig(int drawerId, int& removedScheduleCount) {
    removedScheduleCount = 0;

    Drawer updatedDrawers[MAX_DRAWERS];
    int drawerCount = drawerManager.getDrawers(updatedDrawers, MAX_DRAWERS);
    bool foundDrawer = false;
    for (int index = 0; index < drawerCount; index++) {
        if (updatedDrawers[index].getId() == drawerId) {
            updatedDrawers[index].setMedicationName("");
            updatedDrawers[index].setEnabled(false);
            updatedDrawers[index].setPillCount(0);
            foundDrawer = true;
            break;
        }
    }

    if (!foundDrawer) {
        return false;
    }

    Schedule currentSchedules[MAX_SCHEDULES];
    int currentScheduleCount = reminderController.getSchedules(currentSchedules, MAX_SCHEDULES);

    Schedule keptSchedules[MAX_SCHEDULES];
    int keptScheduleCount = 0;
    int schedulesToRemove = 0;
    for (int index = 0; index < currentScheduleCount; index++) {
        if (currentSchedules[index].getDrawerId() == drawerId) {
            schedulesToRemove++;
            continue;
        }

        keptSchedules[keptScheduleCount++] = currentSchedules[index];
    }

    if (!storageManager.saveDrawersAndSchedules(
        updatedDrawers,
        drawerCount,
        keptSchedules,
        keptScheduleCount
    )) {
        return false;
    }

    removedScheduleCount = schedulesToRemove;
    if (!drawerManager.configureDrawer(drawerId, "", false, 0)) {
        return false;
    }

    drawerManager.stopHighlight(drawerId);
    reminderController.setSchedules(keptSchedules, keptScheduleCount);
    return true;
}

bool DeviceController::applySchedule(const Schedule& schedule) {
    Schedule previousSchedules[MAX_SCHEDULES];
    int previousScheduleCount = reminderController.getSchedules(previousSchedules, MAX_SCHEDULES);

    bool scheduleApplied = reminderController.updateSchedule(schedule);
    if (!scheduleApplied) {
        scheduleApplied = reminderController.addSchedule(schedule);
    }

    if (!scheduleApplied) {
        return false;
    }

    if (persistSchedules()) {
        return true;
    }

    reminderController.setSchedules(previousSchedules, previousScheduleCount);
    return false;
}

bool DeviceController::removeSchedule(int scheduleId) {
    Schedule previousSchedules[MAX_SCHEDULES];
    int previousScheduleCount = reminderController.getSchedules(previousSchedules, MAX_SCHEDULES);

    if (!reminderController.removeSchedule(scheduleId)) {
        return false;
    }

    if (persistSchedules()) {
        return true;
    }

    reminderController.setSchedules(previousSchedules, previousScheduleCount);
    return false;
}

bool DeviceController::acknowledgeEvents(const int eventIds[], int count) {
    if (!storageManager.clearAcknowledgedEvents(eventIds, count)) {
        return false;
    }

    for (int index = unacknowledgedEventCount - 1; index >= 0; index--) {
        int eventId = unacknowledgedEvents[index].getId();
        bool acknowledged = false;
        for (int eventIdIndex = 0; eventIdIndex < count; eventIdIndex++) {
            if (eventIds[eventIdIndex] == eventId) {
                acknowledged = true;
                break;
            }
        }

        if (!acknowledged) {
            continue;
        }

        for (int shift = index; shift < unacknowledgedEventCount - 1; shift++) {
            unacknowledgedEvents[shift] = unacknowledgedEvents[shift + 1];
        }
        unacknowledgedEventCount--;
    }

    return true;
}

bool DeviceController::resetConfiguration() {
    if (!storageManager.clearConfig()) {
        return false;
    }

    drawerManager.turnOffAllDrawers();
    drawerManager.reset();
    for (int index = 0; index < drawerManager.getDrawerCount(); index++) {
        Drawer* drawer = drawerManager.getDrawer(index + 1);
        if (drawer != nullptr) {
            drawer->begin();
        }
    }

    reminderController.reset();
    reminderController.setDrawerManager(&drawerManager);
    unacknowledgedEventCount = 0;
    buzzer.deactivate();
    lcdScreen.showMessage("Config reset");
    return true;
}

void DeviceController::updateReminderFeedback() {
    if (!drawerManager.isHighlightActive()) {
        if (buzzer.isActive()) {
            buzzer.deactivate();
        }
        return;
    }

    if (drawerManager.isHighlightPulseOn()) {
        if (!buzzer.isActive()) {
            buzzer.activate();
        }
        return;
    }

    if (buzzer.isActive()) {
        buzzer.deactivate();
    }
}

void DeviceController::onReminderEvent(const DoseEvent& event) {
    publishDoseEvent(event);
}

void DeviceController::publishDoseEvent(const DoseEvent& event) {
    switch (event.getType()) {
        case DoseEventType::ReminderStarted:
            drawerManager.highlightDrawer(event.getDrawerId());
            lcdScreen.showReminder(event.getMedicationName(), event.getDrawerId());
            break;
        case DoseEventType::DrawerOpened:
            lcdScreen.showMessage(String("Opened drawer ") + String(event.getDrawerId()));
            break;
        case DoseEventType::DrawerClosed:
            lcdScreen.showMessage(String("Closed drawer ") + String(event.getDrawerId()));
            break;
        case DoseEventType::DoseCompleted:
            drawerManager.stopHighlight(event.getDrawerId());
            buzzer.deactivate();
            lcdScreen.showDoseConfirmed();
            break;
        case DoseEventType::DoseMissed:
            drawerManager.stopHighlight(event.getDrawerId());
            buzzer.deactivate();
            lcdScreen.showDoseMissed();
            break;
        case DoseEventType::DrawerEmpty:
            lcdScreen.showMessage(String("Drawer ") + String(event.getDrawerId()) + " empty");
            break;
        default:
            break;
    }

    webSocketService.sendEvent(event);
    if (!storeUnacknowledgedEvent(event)) {
        Serial.println("Failed to store unacknowledged event");
    }

    if (event.getType() == DoseEventType::DoseCompleted) {
        handleDoseCompletedInventory(event);
    }
}

void DeviceController::handleDoseCompletedInventory(const DoseEvent& event) {
    bool drawerBecameEmpty = false;
    if (!drawerManager.recordDoseTaken(event.getDrawerId(), drawerBecameEmpty)) {
        return;
    }

    if (!persistDrawers()) {
        Serial.println("Failed to persist updated pill count");
    }

    if (drawerBecameEmpty) {
        publishDrawerEmptyEvent(event);
    }
}

void DeviceController::publishDrawerEmptyEvent(const DoseEvent& sourceEvent) {
    DoseEvent drawerEmptyEvent(
        reminderController.reserveNextEventId(),
        DoseEventType::DrawerEmpty,
        sourceEvent.getScheduleId(),
        sourceEvent.getScheduledTime(),
        sourceEvent.getDrawerId(),
        sourceEvent.getMedicationName(),
        sourceEvent.getTimestamp(),
        DoseEventStatus::Pending
    );

    publishDoseEvent(drawerEmptyEvent);
}

void DeviceController::loadStoredConfiguration() {
    Drawer storedDrawers[MAX_DRAWERS];
    Schedule storedSchedules[MAX_SCHEDULES];
    DoseEvent storedEvents[MAX_PENDING_EVENTS];

    int drawerCount = storageManager.loadDrawers(storedDrawers, MAX_DRAWERS);
    if (drawerCount > 0) {
        drawerManager.setDrawers(storedDrawers, drawerCount);
    }

    int scheduleCount = storageManager.loadSchedules(storedSchedules, MAX_SCHEDULES);
    if (scheduleCount > 0) {
        reminderController.setSchedules(storedSchedules, scheduleCount);
    }

    unacknowledgedEventCount = storageManager.loadEvents(storedEvents, MAX_PENDING_EVENTS);
    int nextEventId = 1;
    for (int index = 0; index < unacknowledgedEventCount; index++) {
        unacknowledgedEvents[index] = storedEvents[index];
        if (storedEvents[index].getId() >= nextEventId) {
            nextEventId = storedEvents[index].getId() + 1;
        }
    }
    reminderController.setNextEventId(nextEventId);
}

bool DeviceController::persistDrawers() {
    Drawer drawers[MAX_DRAWERS];
    int count = drawerManager.getDrawers(drawers, MAX_DRAWERS);
    return storageManager.saveDrawers(drawers, count);
}

bool DeviceController::persistSchedules() {
    Schedule schedules[MAX_SCHEDULES];
    int count = reminderController.getSchedules(schedules, MAX_SCHEDULES);
    return storageManager.saveSchedules(schedules, count);
}

bool DeviceController::persistEvents() {
    return storageManager.saveEvents(unacknowledgedEvents, unacknowledgedEventCount);
}

bool DeviceController::storeUnacknowledgedEvent(const DoseEvent& event) {
    DoseEvent previousEvents[MAX_PENDING_EVENTS];
    int previousEventCount = unacknowledgedEventCount;
    for (int index = 0; index < previousEventCount; index++) {
        previousEvents[index] = unacknowledgedEvents[index];
    }

    if (unacknowledgedEventCount >= MAX_PENDING_EVENTS) {
        Serial.println("Unacknowledged event buffer full; dropping oldest event");
        for (int index = 0; index < MAX_PENDING_EVENTS - 1; index++) {
            unacknowledgedEvents[index] = unacknowledgedEvents[index + 1];
        }
        unacknowledgedEventCount = MAX_PENDING_EVENTS - 1;
    }

    unacknowledgedEvents[unacknowledgedEventCount++] = event;
    if (persistEvents()) {
        return true;
    }

    unacknowledgedEventCount = previousEventCount;
    for (int index = 0; index < previousEventCount; index++) {
        unacknowledgedEvents[index] = previousEvents[index];
    }
    return false;
}
