#include "communication/WebServerController.h"

#include <ArduinoJson.h>

#include "config/SystemConfig.h"
#include "controllers/DeviceController.h"
#include "utils/DateTimeValidation.h"
#include "utils/DoseEventJson.h"

namespace {

constexpr const char* DEVICE_NAME = "Smart Pill Reminder Box";
constexpr const char* FIRMWARE_VERSION = "1.0.0";

String serializeJsonDocument(const JsonDocument& doc) {
    String response;
    serializeJson(doc, response);
    return response;
}

String buildErrorResponse(const String& error, const String& message) {
    JsonDocument doc;
    doc["success"] = false;
    doc["error"] = error;
    doc["message"] = message;
    return serializeJsonDocument(doc);
}

void fillDrawerJson(JsonObject drawerObject, const Drawer& drawer) {
    drawerObject["id"] = drawer.getId();
    drawerObject["medicationName"] = drawer.getMedicationName();
    drawerObject["enabled"] = drawer.isEnabled();
}

void fillScheduleJson(JsonObject scheduleObject, const Schedule& schedule) {
    scheduleObject["id"] = schedule.getId();
    scheduleObject["drawerId"] = schedule.getDrawerId();
    scheduleObject["enabled"] = schedule.isEnabled();

    JsonArray timesArray = scheduleObject["times"].to<JsonArray>();
    for (int timeIndex = 0; timeIndex < schedule.getTimeCount(); timeIndex++) {
        ScheduleTime time = schedule.getTime(timeIndex);
        if (time.hour < 0 || time.minute < 0) {
            continue;
        }

        JsonObject timeObject = timesArray.add<JsonObject>();
        timeObject["hour"] = time.hour;
        timeObject["minute"] = time.minute;
    }

    JsonArray daysArray = scheduleObject["daysOfWeek"].to<JsonArray>();
    for (int dayIndex = 0; dayIndex < schedule.getDayCount(); dayIndex++) {
        String dayOfWeek = schedule.getDayOfWeek(dayIndex);
        if (dayOfWeek.length() == 0) {
            continue;
        }

        daysArray.add(dayOfWeek);
    }
}

void fillEventJson(JsonObject eventObject, const DoseEvent& event) {
    eventObject["id"] = event.getId();
    eventObject["type"] = doseEventTypeToString(event.getType());
    eventObject["scheduleId"] = event.getScheduleId();
    eventObject["scheduledTime"] = event.getScheduledTime();
    eventObject["drawerId"] = event.getDrawerId();
    eventObject["medicationName"] = event.getMedicationName();
    eventObject["timestamp"] = event.getTimestamp();
    if (event.getStatus() != DoseEventStatus::Pending) {
        eventObject["status"] = doseEventStatusToString(event.getStatus());
    }
}

bool parseScheduleFromBody(
    const String& requestBody,
    int scheduleId,
    Schedule& schedule,
    String& errorResponse
) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);
    if (error) {
        errorResponse = buildErrorResponse("INVALID_SCHEDULE", "The provided schedule payload is invalid");
        return false;
    }

    int drawerId = doc["drawerId"] | 0;
    if (drawerId <= 0) {
        errorResponse = buildErrorResponse("INVALID_DRAWER_ID", "Schedule references an invalid drawer");
        return false;
    }

    JsonArray timesArray = doc["times"].as<JsonArray>();
    if (timesArray.isNull() || timesArray.size() == 0) {
        errorResponse = buildErrorResponse("INVALID_SCHEDULE", "Schedule must include at least one time");
        return false;
    }

    schedule = Schedule(scheduleId, drawerId);
    schedule.setEnabled(doc["enabled"] | false);

    for (JsonObject timeObject : timesArray) {
        int hour = timeObject["hour"] | -1;
        int minute = timeObject["minute"] | -1;
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
            errorResponse = buildErrorResponse("INVALID_SCHEDULE", "Schedule contains an invalid time");
            return false;
        }

        if (!schedule.addTime(hour, minute)) {
            errorResponse = buildErrorResponse("INVALID_SCHEDULE", "Schedule contains too many times");
            return false;
        }
    }

    JsonArray daysArray = doc["daysOfWeek"].as<JsonArray>();
    if (!daysArray.isNull()) {
        for (JsonVariant dayValue : daysArray) {
            String dayOfWeek = dayValue.as<String>();
            if (dayOfWeek.length() == 0) {
                errorResponse = buildErrorResponse("INVALID_SCHEDULE", "Schedule contains an invalid day");
                return false;
            }

            if (!schedule.addDayOfWeek(dayOfWeek)) {
                errorResponse = buildErrorResponse("INVALID_SCHEDULE", "Schedule contains too many days");
                return false;
            }
        }
    }

    return true;
}

}  // namespace

WebServerController::WebServerController(DeviceController* deviceController)
    : deviceController(deviceController), server(80), lastResponse(""), wifiReady(false) {}

void WebServerController::begin() {
    wifiReady = startAccessPoint();

    server.on("/api/health", HTTP_GET, [this]() {
        handleGetHealth();
        sendJsonResponse();
    });

    server.on("/api/drawers", HTTP_GET, [this]() {
        handleGetDrawers();
        sendJsonResponse();
    });

    server.on("/api/schedules", HTTP_GET, [this]() {
        handleGetSchedules();
        sendJsonResponse();
    });

    server.on("/api/schedules", HTTP_POST, [this]() {
        handleCreateSchedule(server.arg("plain"));
        sendJsonResponse();
    });

    server.on("/api/events", HTTP_GET, [this]() {
        handleGetEvents();
        sendJsonResponse();
    });

    server.on("/api/events/ack", HTTP_POST, [this]() {
        handleAcknowledgeEvents(server.arg("plain"));
        sendJsonResponse();
    });

    server.on("/api/time", HTTP_GET, [this]() {
        handleGetTime();
        sendJsonResponse();
    });

    server.on("/api/time", HTTP_PUT, [this]() {
        handleSetTime(server.arg("plain"));
        sendJsonResponse();
    });

    server.onNotFound([this]() {
        handleDynamicRoute();
    });

    server.begin();
}

void WebServerController::handleClient() {
    server.handleClient();
}

void WebServerController::loop() {
    handleClient();
}

bool WebServerController::isWifiReady() const {
    return wifiReady;
}

void WebServerController::sendJsonResponse(int statusCode) {
    server.send(statusCode, "application/json", lastResponse);
}

bool WebServerController::extractPathId(const String& uri, const String& prefix, int& id) const {
    if (!uri.startsWith(prefix)) {
        return false;
    }

    String idPart = uri.substring(prefix.length());
    if (idPart.length() == 0 || idPart.indexOf('/') != -1 || !isDigits(idPart)) {
        return false;
    }

    id = idPart.toInt();
    return id > 0;
}

bool WebServerController::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    return WiFi.softAP(DEVICE_AP_SSID, DEVICE_AP_PASSWORD);
}

void WebServerController::handleDynamicRoute() {
    int id = 0;
    String uri = server.uri();
    HTTPMethod method = server.method();

    if (method == HTTP_PUT && extractPathId(uri, "/api/drawers/", id)) {
        handleUpdateDrawer(id, server.arg("plain"));
        sendJsonResponse();
        return;
    }

    if (method == HTTP_PUT && extractPathId(uri, "/api/schedules/", id)) {
        handleUpdateSchedule(id, server.arg("plain"));
        sendJsonResponse();
        return;
    }

    if (method == HTTP_DELETE && extractPathId(uri, "/api/schedules/", id)) {
        handleDeleteSchedule(id);
        sendJsonResponse();
        return;
    }

    lastResponse = buildErrorResponse("NOT_FOUND", "Endpoint not found");
    sendJsonResponse(404);
}

void WebServerController::handleGetHealth() {
    if (deviceController == nullptr || !deviceController->isReady()) {
        lastResponse = buildErrorResponse(
            "DEVICE_NOT_READY",
            "Device services are not fully initialized"
        );
        return;
    }

    JsonDocument doc;
    doc["success"] = true;
    doc["deviceName"] = DEVICE_NAME;
    doc["firmwareVersion"] = FIRMWARE_VERSION;
    doc["wifiConnected"] = deviceController->isWifiConnected();
    doc["webSocketEnabled"] = deviceController->isWebSocketEnabled();
    doc["storageReady"] = deviceController->isStorageReady();
    lastResponse = serializeJsonDocument(doc);
}

void WebServerController::handleGetDrawers() {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "DRAWERS_UNAVAILABLE",
            "Drawer configuration could not be loaded"
        );
        return;
    }

    Drawer drawers[MAX_DRAWERS];
    int count = deviceController->getDrawers(drawers, MAX_DRAWERS);

    JsonDocument doc;
    doc["success"] = true;
    JsonArray drawersArray = doc["drawers"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject drawerObject = drawersArray.add<JsonObject>();
        fillDrawerJson(drawerObject, drawers[index]);
    }

    lastResponse = serializeJsonDocument(doc);
}

void WebServerController::handleGetSchedules() {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "SCHEDULES_UNAVAILABLE",
            "Schedule configuration could not be loaded"
        );
        return;
    }

    Schedule schedules[MAX_SCHEDULES];
    int count = deviceController->getSchedules(schedules, MAX_SCHEDULES);

    JsonDocument doc;
    doc["success"] = true;
    JsonArray schedulesArray = doc["schedules"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject scheduleObject = schedulesArray.add<JsonObject>();
        fillScheduleJson(scheduleObject, schedules[index]);
    }

    lastResponse = serializeJsonDocument(doc);
}

void WebServerController::handleGetEvents() {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "EVENTS_UNAVAILABLE",
            "Unacknowledged events could not be loaded"
        );
        return;
    }

    DoseEvent events[MAX_PENDING_EVENTS];
    int count = deviceController->getUnacknowledgedEvents(events, MAX_PENDING_EVENTS);

    JsonDocument doc;
    doc["success"] = true;
    JsonArray eventsArray = doc["events"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject eventObject = eventsArray.add<JsonObject>();
        fillEventJson(eventObject, events[index]);
    }

    lastResponse = serializeJsonDocument(doc);
}

void WebServerController::handleGetTime() {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse("TIME_UNAVAILABLE", "Device time is not configured");
        return;
    }

    DateTime currentDateTime = deviceController->getCurrentDateTime();
    if (currentDateTime.date.length() == 0 || currentDateTime.dayOfWeek.length() == 0) {
        lastResponse = buildErrorResponse("TIME_UNAVAILABLE", "Device time is not configured");
        return;
    }

    JsonDocument doc;
    doc["success"] = true;
    doc["currentDate"] = currentDateTime.date;
    doc["currentTime"] = currentDateTime.time;
    doc["dayOfWeek"] = currentDateTime.dayOfWeek;
    lastResponse = serializeJsonDocument(doc);
}

void WebServerController::handleUpdateDrawer(int drawerId, const String& requestBody) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "DRAWER_NOT_FOUND",
            "Drawer " + String(drawerId) + " does not exist"
        );
        return;
    }

    Drawer drawer;
    if (!deviceController->getDrawer(drawerId, drawer)) {
        lastResponse = buildErrorResponse(
            "DRAWER_NOT_FOUND",
            "Drawer " + String(drawerId) + " does not exist"
        );
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);
    if (error) {
        lastResponse = buildErrorResponse("INVALID_DRAWER", "The provided drawer payload is invalid");
        return;
    }

    String medicationName = doc["medicationName"] | "";
    bool enabled = doc["enabled"] | false;
    if (!deviceController->applyDrawerConfig(drawerId, medicationName, enabled)) {
        lastResponse = buildErrorResponse(
            "DRAWER_NOT_FOUND",
            "Drawer " + String(drawerId) + " does not exist"
        );
        return;
    }

    deviceController->getDrawer(drawerId, drawer);

    JsonDocument responseDoc;
    responseDoc["success"] = true;
    responseDoc["message"] = "Drawer updated";
    JsonObject drawerObject = responseDoc["drawer"].to<JsonObject>();
    fillDrawerJson(drawerObject, drawer);
    lastResponse = serializeJsonDocument(responseDoc);
}

void WebServerController::handleCreateSchedule(const String& requestBody) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "INVALID_DRAWER_ID",
            "Schedule references an invalid drawer"
        );
        return;
    }

    handleSaveSchedule(deviceController->getNextScheduleId(), requestBody, false);
}

void WebServerController::handleUpdateSchedule(int scheduleId, const String& requestBody) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "SCHEDULE_NOT_FOUND",
            "Schedule " + String(scheduleId) + " does not exist"
        );
        return;
    }

    Schedule existingSchedule;
    if (!deviceController->getSchedule(scheduleId, existingSchedule)) {
        lastResponse = buildErrorResponse(
            "SCHEDULE_NOT_FOUND",
            "Schedule " + String(scheduleId) + " does not exist"
        );
        return;
    }

    handleSaveSchedule(scheduleId, requestBody, true);
}

void WebServerController::handleSaveSchedule(int scheduleId, const String& requestBody, bool isUpdate) {
    Schedule schedule;
    String errorResponse;
    if (!parseScheduleFromBody(requestBody, scheduleId, schedule, errorResponse)) {
        lastResponse = errorResponse;
        return;
    }

    Drawer drawer;
    if (!deviceController->getDrawer(schedule.getDrawerId(), drawer)) {
        lastResponse = buildErrorResponse(
            "INVALID_DRAWER_ID",
            "Schedule references drawer " + String(schedule.getDrawerId()) + ", but drawer "
                + String(schedule.getDrawerId()) + " does not exist"
        );
        return;
    }

    if (!deviceController->applySchedule(schedule)) {
        lastResponse = isUpdate
            ? buildErrorResponse(
                "SCHEDULE_NOT_FOUND",
                "Schedule " + String(scheduleId) + " does not exist"
            )
            : buildErrorResponse("INVALID_SCHEDULE", "Schedule could not be created");
        return;
    }

    JsonDocument responseDoc;
    responseDoc["success"] = true;
    responseDoc["message"] = isUpdate ? "Schedule updated" : "Schedule created";
    JsonObject scheduleObject = responseDoc["schedule"].to<JsonObject>();
    fillScheduleJson(scheduleObject, schedule);
    lastResponse = serializeJsonDocument(responseDoc);
}

void WebServerController::handleDeleteSchedule(int scheduleId) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "SCHEDULE_NOT_FOUND",
            "Schedule " + String(scheduleId) + " does not exist"
        );
        return;
    }

    Schedule schedule;
    if (!deviceController->getSchedule(scheduleId, schedule)) {
        lastResponse = buildErrorResponse(
            "SCHEDULE_NOT_FOUND",
            "Schedule " + String(scheduleId) + " does not exist"
        );
        return;
    }

    if (!deviceController->removeSchedule(scheduleId)) {
        lastResponse = buildErrorResponse(
            "SCHEDULE_NOT_FOUND",
            "Schedule " + String(scheduleId) + " does not exist"
        );
        return;
    }

    JsonDocument responseDoc;
    responseDoc["success"] = true;
    responseDoc["message"] = "Schedule deleted";
    responseDoc["deletedScheduleId"] = scheduleId;
    lastResponse = serializeJsonDocument(responseDoc);
}

void WebServerController::handleSetTime(const String& requestBody) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse("INVALID_TIME", "The provided time format is invalid");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);
    if (error) {
        lastResponse = buildErrorResponse("INVALID_TIME", "The provided time format is invalid");
        return;
    }

    String currentDate = doc["currentDate"] | "";
    String currentTime = doc["currentTime"] | "";
    String dayOfWeek = doc["dayOfWeek"] | "";

    if (!isValidDateString(currentDate) || !isValidTimeString(currentTime) || dayOfWeek.length() == 0) {
        lastResponse = buildErrorResponse("INVALID_TIME", "The provided time format is invalid");
        return;
    }

    deviceController->setCurrentDateTime(currentDate, currentTime, dayOfWeek);

    JsonDocument responseDoc;
    responseDoc["success"] = true;
    responseDoc["message"] = "Device time updated";
    responseDoc["currentDate"] = currentDate;
    responseDoc["currentTime"] = currentTime;
    responseDoc["dayOfWeek"] = dayOfWeek;
    lastResponse = serializeJsonDocument(responseDoc);
}

void WebServerController::handleAcknowledgeEvents(const String& requestBody) {
    if (deviceController == nullptr) {
        lastResponse = buildErrorResponse(
            "INVALID_EVENT_IDS",
            "One or more event IDs do not exist in the unacknowledged event buffer"
        );
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);
    if (error) {
        lastResponse = buildErrorResponse(
            "INVALID_EVENT_IDS",
            "One or more event IDs do not exist in the unacknowledged event buffer"
        );
        return;
    }

    JsonArray eventIdsArray = doc["eventIds"].as<JsonArray>();
    if (eventIdsArray.isNull() || eventIdsArray.size() == 0) {
        lastResponse = buildErrorResponse(
            "INVALID_EVENT_IDS",
            "One or more event IDs do not exist in the unacknowledged event buffer"
        );
        return;
    }

    if (eventIdsArray.size() > MAX_PENDING_EVENTS) {
        lastResponse = buildErrorResponse(
            "INVALID_EVENT_IDS",
            "Too many event IDs were provided"
        );
        return;
    }

    int requestedEventIds[MAX_PENDING_EVENTS];
    int requestedCount = 0;
    for (JsonVariant eventIdValue : eventIdsArray) {
        requestedEventIds[requestedCount++] = eventIdValue.as<int>();
    }

    DoseEvent currentEvents[MAX_PENDING_EVENTS];
    int currentEventCount = deviceController->getUnacknowledgedEvents(currentEvents, MAX_PENDING_EVENTS);
    for (int requestedIndex = 0; requestedIndex < requestedCount; requestedIndex++) {
        bool found = false;
        for (int eventIndex = 0; eventIndex < currentEventCount; eventIndex++) {
            if (currentEvents[eventIndex].getId() == requestedEventIds[requestedIndex]) {
                found = true;
                break;
            }
        }

        if (!found) {
            lastResponse = buildErrorResponse(
                "INVALID_EVENT_IDS",
                "One or more event IDs do not exist in the unacknowledged event buffer"
            );
            return;
        }
    }

    if (!deviceController->acknowledgeEvents(requestedEventIds, requestedCount)) {
        lastResponse = buildErrorResponse(
            "INVALID_EVENT_IDS",
            "One or more event IDs do not exist in the unacknowledged event buffer"
        );
        return;
    }

    JsonDocument responseDoc;
    responseDoc["success"] = true;
    responseDoc["message"] = "Events acknowledged";
    responseDoc["acknowledgedCount"] = requestedCount;
    lastResponse = serializeJsonDocument(responseDoc);
}

String WebServerController::getLastResponse() const {
    return lastResponse;
}
