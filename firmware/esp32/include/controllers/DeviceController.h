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
    bool isReady() const;
    bool isStorageReady() const;
    bool isWifiConnected() const;
    bool isWebSocketEnabled() const;
    int getDrawers(Drawer outputDrawers[], int maxDrawers) const;
    bool getDrawer(int drawerId, Drawer& drawer) const;
    int getSchedules(Schedule outputSchedules[], int maxSchedules) const;
    bool getSchedule(int scheduleId, Schedule& schedule) const;
    int getNextScheduleId() const;
    int getUnacknowledgedEvents(DoseEvent outputEvents[], int maxEvents) const;
    DateTime getCurrentDateTime() const;
    bool setCurrentDateTime(const String& currentDate, const String& currentTime, const String& dayOfWeek);
    bool applyDrawerConfig(int drawerId, const String& medicationName, bool enabled, int pillCount);
    bool deleteDrawerConfig(int drawerId, int& removedScheduleCount);
    bool applySchedule(const Schedule& schedule);
    bool removeSchedule(int scheduleId);
    bool acknowledgeEvents(const int eventIds[], int count);
    bool resetConfiguration();
    void onReminderEvent(const DoseEvent& event);
    void publishDoseEvent(const DoseEvent& event);
    void loadStoredConfiguration();

private:
    bool persistDrawers();
    bool persistSchedules();
    bool persistEvents();
    bool storeUnacknowledgedEvent(const DoseEvent& event);
    void updateReminderFeedback();
    void handleDoseCompletedInventory(const DoseEvent& event);
    void publishDrawerEmptyEvent(const DoseEvent& sourceEvent);

    ReminderController reminderController;
    DrawerManager drawerManager;
    WebServerController webServerController;
    WebSocketService webSocketService;
    StorageManager storageManager;
    LCDScreen lcdScreen;
    Buzzer buzzer;
    ClockModule clockModule;
    DoseEvent unacknowledgedEvents[MAX_PENDING_EVENTS];
    int unacknowledgedEventCount;
    bool storageReady;
};
