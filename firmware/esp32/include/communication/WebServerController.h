#pragma once

#include <Arduino.h>

class DeviceController;

class WebServerController {
public:
    explicit WebServerController(DeviceController* deviceController = nullptr);

    void begin();
    void handleClient();
    void loop();
    void handleGetHealth();
    void handleGetDrawers();
    void handleGetSchedules();
    void handleGetEvents();
    void handleGetTime();
    void handleUpdateDrawer(int drawerId);
    void handleCreateSchedule();
    void handleUpdateSchedule(int scheduleId);
    void handleDeleteSchedule(int scheduleId);
    void handleSetTime();
    void handleAcknowledgeEvents();
    String getLastResponse() const;

private:
    DeviceController* deviceController;
    String lastResponse;
};
