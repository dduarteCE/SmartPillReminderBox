#pragma once

#include <Arduino.h>
#include "communication/WebServerController.h"
#include "communication/WebSocketService.h"
#include "hardware/Buzzer.h"
#include "hardware/ClockModule.h"
#include "hardware/LCDScreen.h"
#include "models/DoseEvent.h"
#include "models/Schedule.h"
#include "storage/StorageManager.h"
#include "controllers/DrawerManager.h"
#include "controllers/ReminderController.h"

class DeviceController {
public:
    DeviceController();

    void setup();
    void loop();
    bool applyDrawerConfig(int drawerId, const String& medicationName, bool enabled);
    bool applySchedule(const Schedule& schedule);
    bool removeSchedule(int scheduleId);
    void onReminderEvent(const DoseEvent& event);
    void publishDoseEvent(const DoseEvent& event);
    void loadStoredConfiguration();

private:
    ReminderController reminderController;
    DrawerManager drawerManager;
    WebServerController webServerController;
    WebSocketService webSocketService;
    StorageManager storageManager;
    LCDScreen lcdScreen;
    Buzzer buzzer;
    ClockModule clockModule;
};
