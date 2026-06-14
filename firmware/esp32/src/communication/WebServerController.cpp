#include "communication/WebServerController.h"

WebServerController::WebServerController(DeviceController* deviceController)
    : deviceController(deviceController), lastResponse("") {}

void WebServerController::begin() {}

void WebServerController::handleClient() {}

void WebServerController::loop() {
    handleClient();
}

void WebServerController::handleGetHealth() {
    lastResponse = "{\"status\":\"ok\"}";
}

void WebServerController::handleGetDrawers() {
    lastResponse = "{\"drawers\":[]}";
}

void WebServerController::handleGetSchedules() {
    lastResponse = "{\"schedules\":[]}";
}

void WebServerController::handleGetEvents() {
    lastResponse = "{\"events\":[]}";
}

void WebServerController::handleGetTime() {
    lastResponse = "{\"date\":\"\",\"time\":\"00:00\",\"dayOfWeek\":\"\"}";
}

void WebServerController::handleUpdateDrawer(int drawerId) {
    (void)drawerId;
    lastResponse = "{\"updated\":true}";
}

void WebServerController::handleCreateSchedule() {
    lastResponse = "{\"created\":true}";
}

void WebServerController::handleUpdateSchedule(int scheduleId) {
    (void)scheduleId;
    lastResponse = "{\"updated\":true}";
}

void WebServerController::handleDeleteSchedule(int scheduleId) {
    (void)scheduleId;
    lastResponse = "{\"deleted\":true}";
}

void WebServerController::handleSetTime() {
    lastResponse = "{\"updated\":true}";
}

void WebServerController::handleAcknowledgeEvents() {
    lastResponse = "{\"acknowledged\":true}";
}

String WebServerController::getLastResponse() const {
    return lastResponse;
}
