#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

class DeviceController;

class WebServerController {
public:
    explicit WebServerController(DeviceController* deviceController = nullptr);

    void begin();
    void handleClient();
    void loop();
    bool isWifiReady() const;
    void handleGetHealth();
    void handleGetDrawers();
    void handleGetSchedules();
    void handleGetEvents();
    void handleGetTime();
    void handleUpdateDrawer(int drawerId, const String& requestBody = "");
    void handleCreateSchedule(const String& requestBody = "");
    void handleUpdateSchedule(int scheduleId, const String& requestBody = "");
    void handleDeleteSchedule(int scheduleId);
    void handleSetTime(const String& requestBody = "");
    void handleAcknowledgeEvents(const String& requestBody = "");
    String getLastResponse() const;

private:
    void sendJsonResponse(int statusCode = 200);
    void handleDynamicRoute();
    void handleSaveSchedule(int scheduleId, const String& requestBody, bool isUpdate);
    bool extractPathId(const String& uri, const String& prefix, int& id) const;
    bool startAccessPoint();

    DeviceController* deviceController;
    WebServer server;
    String lastResponse;
    bool wifiReady;
};
