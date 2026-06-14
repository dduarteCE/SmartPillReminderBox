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
      clockModule() {}

void DeviceController::setup() {
    storageManager.begin();
    drawerManager.begin();
    reminderController.begin();
    lcdScreen.begin();
    buzzer.begin();
    clockModule.begin();
    webServerController.begin();
    webSocketService.begin();
    loadStoredConfiguration();
}

void DeviceController::loop() {
    webServerController.handleClient();
    webSocketService.handleWebSocket();
    drawerManager.update();
    reminderController.update();

    while (reminderController.hasPendingEvent()) {
        publishDoseEvent(reminderController.popPendingEvent());
    }
}

bool DeviceController::applyDrawerConfig(int drawerId, const String& medicationName, bool enabled) {
    return drawerManager.configureDrawer(drawerId, medicationName, enabled);
}

bool DeviceController::applySchedule(const Schedule& schedule) {
    if (reminderController.updateSchedule(schedule)) {
        return true;
    }

    return reminderController.addSchedule(schedule);
}

bool DeviceController::removeSchedule(int scheduleId) {
    return reminderController.removeSchedule(scheduleId);
}

void DeviceController::onReminderEvent(const DoseEvent& event) {
    publishDoseEvent(event);
}

void DeviceController::publishDoseEvent(const DoseEvent& event) {
    webSocketService.sendEvent(event);
}

void DeviceController::loadStoredConfiguration() {
    Drawer storedDrawers[MAX_DRAWERS];
    Schedule storedSchedules[MAX_SCHEDULES];

    int drawerCount = storageManager.loadDrawers(storedDrawers, MAX_DRAWERS);
    if (drawerCount > 0) {
        drawerManager.setDrawers(storedDrawers, drawerCount);
    }

    int scheduleCount = storageManager.loadSchedules(storedSchedules, MAX_SCHEDULES);
    if (scheduleCount > 0) {
        reminderController.setSchedules(storedSchedules, scheduleCount);
    }
}
