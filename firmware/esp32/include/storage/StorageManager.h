#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config/SystemConfig.h"
#include "models/DoseEvent.h"
#include "models/Drawer.h"
#include "models/Schedule.h"

class StorageManager {
public:
    StorageManager(const String& configFilePath = CONFIG_FILE_PATH);

    bool begin();
    int loadDrawers(Drawer drawers[], int maxDrawers);
    bool saveDrawers(const Drawer drawers[], int count);
    bool saveDrawersAndSchedules(
        const Drawer drawers[],
        int drawerCount,
        const Schedule schedules[],
        int scheduleCount
    );
    int loadSchedules(Schedule schedules[], int maxSchedules);
    bool saveSchedules(const Schedule schedules[], int count);
    int loadEvents(DoseEvent events[], int maxEvents);
    bool saveEvents(const DoseEvent events[], int count);
    bool clearAcknowledgedEvents(const int eventIds[], int count);
    bool clearConfig();
    String getConfigFilePath() const;

private:
    bool readConfigDocument(JsonDocument& doc);
    bool writeConfigDocument(const JsonDocument& doc);

    String configFilePath;
};
