#include <LittleFS.h>
#include <ArduinoJson.h>

#include "config/PinConfig.h"
#include "storage/StorageManager.h"

StorageManager::StorageManager(const String& configFilePath) : configFilePath(configFilePath) {}

bool StorageManager::begin() {
    return LittleFS.begin(true);
}

bool StorageManager::readConfigDocument(JsonDocument& doc) {
    File file = LittleFS.open(configFilePath, "r");
    if (!file) {
        Serial.println("No saved config yet");
        return false;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.println("Failed to parse config.json");
        return false;
    }

    return true;
}

bool StorageManager::writeConfigDocument(const JsonDocument& doc) {
    File file = LittleFS.open(configFilePath, "w");
    if (!file) {
        Serial.println("Failed to open config.json for writing");
        return false;
    }

    size_t bytesWritten = serializeJson(doc, file);
    file.close();
    if (bytesWritten == 0) {
        Serial.println("Failed to write config.json");
        return false;
    }

    return true;
}

bool mapDrawerPins(int drawerid, int& ledPin, int& reedSwitchPin) {
    ledPin = -1;
    reedSwitchPin = -1;
    switch (drawerid) {
        case 1:
            ledPin = LED_DRAWER_1_PIN;
            reedSwitchPin = REED_DRAWER_1_PIN;
            return true;
        case 2:
            ledPin = LED_DRAWER_2_PIN;
            reedSwitchPin = REED_DRAWER_2_PIN;
            return true;
        case 3:
            ledPin = LED_DRAWER_3_PIN;
            reedSwitchPin = REED_DRAWER_3_PIN;
            return true;
        case 4:
            ledPin = LED_DRAWER_4_PIN;
            reedSwitchPin = REED_DRAWER_4_PIN;
            return true;
        case 5:
            ledPin = LED_DRAWER_5_PIN;
            reedSwitchPin = REED_DRAWER_5_PIN;
            return true;
        case 6:
            ledPin = LED_DRAWER_6_PIN;
            reedSwitchPin = REED_DRAWER_6_PIN;
            return true;
        case 7:
            ledPin = LED_DRAWER_7_PIN;
            reedSwitchPin = REED_DRAWER_7_PIN;
            return true;
        default:
            return false;
    }
}

String eventTypeToString(DoseEventType type) {
    switch (type) {
        case DoseEventType::DrawerOpened:
            return "DRAWER_OPENED";
        case DoseEventType::DrawerClosed:
            return "DRAWER_CLOSED";
        case DoseEventType::DoseCompleted:
            return "DOSE_COMPLETED";
        case DoseEventType::DoseMissed:
            return "DOSE_MISSED";
        case DoseEventType::ReminderStarted:
        default:
            return "REMINDER_STARTED";
    }
}

DoseEventType eventTypeFromString(const String& type) {
    if (type == "DRAWER_OPENED") {
        return DoseEventType::DrawerOpened;
    }
    if (type == "DRAWER_CLOSED") {
        return DoseEventType::DrawerClosed;
    }
    if (type == "DOSE_COMPLETED") {
        return DoseEventType::DoseCompleted;
    }
    if (type == "DOSE_MISSED") {
        return DoseEventType::DoseMissed;
    }

    return DoseEventType::ReminderStarted;
}

String eventStatusToString(DoseEventStatus status) {
    switch (status) {
        case DoseEventStatus::Taken:
            return "TAKEN";
        case DoseEventStatus::Missed:
            return "MISSED";
        case DoseEventStatus::Pending:
        default:
            return "PENDING";
    }
}

DoseEventStatus eventStatusFromString(const String& status) {
    if (status == "TAKEN") {
        return DoseEventStatus::Taken;
    }
    if (status == "MISSED") {
        return DoseEventStatus::Missed;
    }

    return DoseEventStatus::Pending;
}

bool containsEventId(const int eventIds[], int count, int eventId) {
    for (int index = 0; index < count; index++) {
        if (eventIds[index] == eventId) {
            return true;
        }
    }

    return false;
}

int StorageManager::loadDrawers(Drawer drawers[], int maxDrawers) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        return 0;
    }

    JsonArray drawersArray = doc["drawers"].as<JsonArray>();
    if (drawersArray.isNull()) {
        Serial.println("No drawers array in config");
        return 0;
    }

    int count = 0;

    for (JsonObject drawer : drawersArray) {
        if (count >= maxDrawers) {
            break;
        }

        int id = drawer["id"] | 0;
        if (id <= 0) {
            continue;
        }

        String medicationName = drawer["medicationName"] | "";
        bool enabled = drawer["enabled"] | false;
        int ledPin;
        int reedSwitchPin;
        if (!mapDrawerPins(id, ledPin, reedSwitchPin)) {
            continue;
        }

        drawers[count] = Drawer(id, medicationName, enabled, ledPin, reedSwitchPin);
        count++;
    }

    return count;
}

bool StorageManager::saveDrawers(const Drawer drawers[], int count) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        doc.clear();
    }

    JsonArray drawersArray = doc["drawers"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject drawerObject = drawersArray.add<JsonObject>();
        drawerObject["id"] = drawers[index].getId();
        drawerObject["medicationName"] = drawers[index].getMedicationName();
        drawerObject["enabled"] = drawers[index].isEnabled();
    }

    return writeConfigDocument(doc);
}

int StorageManager::loadSchedules(Schedule schedules[], int maxSchedules) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        return 0;
    }

    JsonArray schedulesArray = doc["schedules"].as<JsonArray>();
    if (schedulesArray.isNull()) {
        Serial.println("No schedules array in config");
        return 0;
    }

    int count = 0;

    for (JsonObject scheduleObject : schedulesArray) {
        if (count >= maxSchedules) {
            break;
        }

        int id = scheduleObject["id"] | 0;
        int drawerId = scheduleObject["drawerId"] | 0;
        if (id <= 0 || drawerId <= 0) {
            continue;
        }

        Schedule schedule(id, drawerId);
        schedule.setEnabled(scheduleObject["enabled"] | false);

        JsonArray timesArray = scheduleObject["times"].as<JsonArray>();
        if (!timesArray.isNull()) {
            for (JsonObject timeObject : timesArray) {
                int hour = timeObject["hour"] | -1;
                int minute = timeObject["minute"] | -1;
                if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                    continue;
                }

                schedule.addTime(hour, minute);
            }
        }

        JsonArray daysOfWeekArray = scheduleObject["daysOfWeek"].as<JsonArray>();
        if (!daysOfWeekArray.isNull()) {
            for (JsonVariant dayValue : daysOfWeekArray) {
                String dayOfWeek = dayValue.as<String>();
                if (dayOfWeek.length() == 0) {
                    continue;
                }

                schedule.addDayOfWeek(dayOfWeek);
            }
        }

        schedules[count] = schedule;
        count++;
    }

    return count;
}

bool StorageManager::saveSchedules(const Schedule schedules[], int count) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        doc.clear();
    }

    JsonArray schedulesArray = doc["schedules"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject scheduleObject = schedulesArray.add<JsonObject>();
        scheduleObject["id"] = schedules[index].getId();
        scheduleObject["drawerId"] = schedules[index].getDrawerId();
        scheduleObject["enabled"] = schedules[index].isEnabled();

        JsonArray timesArray = scheduleObject["times"].to<JsonArray>();
        for (int timeIndex = 0; timeIndex < schedules[index].getTimeCount(); timeIndex++) {
            ScheduleTime time = schedules[index].getTime(timeIndex);
            if (time.hour < 0 || time.minute < 0) {
                continue;
            }

            JsonObject timeObject = timesArray.add<JsonObject>();
            timeObject["hour"] = time.hour;
            timeObject["minute"] = time.minute;
        }

        JsonArray daysOfWeekArray = scheduleObject["daysOfWeek"].to<JsonArray>();
        for (int dayIndex = 0; dayIndex < schedules[index].getDayCount(); dayIndex++) {
            String dayOfWeek = schedules[index].getDayOfWeek(dayIndex);
            if (dayOfWeek.length() == 0) {
                continue;
            }

            daysOfWeekArray.add(dayOfWeek);
        }
    }

    return writeConfigDocument(doc);
}

int StorageManager::loadEvents(DoseEvent events[], int maxEvents) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        return 0;
    }

    JsonArray eventsArray = doc["events"].as<JsonArray>();
    if (eventsArray.isNull()) {
        Serial.println("No events array in config");
        return 0;
    }

    int count = 0;

    for (JsonObject eventObject : eventsArray) {
        if (count >= maxEvents) {
            break;
        }

        int id = eventObject["id"] | -1;
        int scheduleId = eventObject["scheduleId"] | 0;
        int drawerId = eventObject["drawerId"] | 0;
        if (id < 0 || scheduleId <= 0 || drawerId <= 0) {
            continue;
        }

        String type = eventObject["type"] | "REMINDER_STARTED";
        String scheduledTime = eventObject["scheduledTime"] | "";
        String medicationName = eventObject["medicationName"] | "";
        String timestamp = eventObject["timestamp"] | "";
        String status = eventObject["status"] | "PENDING";

        events[count] = DoseEvent(
            id,
            eventTypeFromString(type),
            scheduleId,
            scheduledTime,
            drawerId,
            medicationName,
            timestamp,
            eventStatusFromString(status)
        );
        count++;
    }

    return count;
}

bool StorageManager::saveEvents(const DoseEvent events[], int count) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        doc.clear();
    }

    JsonArray eventsArray = doc["events"].to<JsonArray>();
    for (int index = 0; index < count; index++) {
        JsonObject eventObject = eventsArray.add<JsonObject>();
        eventObject["id"] = events[index].getId();
        eventObject["type"] = eventTypeToString(events[index].getType());
        eventObject["scheduleId"] = events[index].getScheduleId();
        eventObject["scheduledTime"] = events[index].getScheduledTime();
        eventObject["drawerId"] = events[index].getDrawerId();
        eventObject["medicationName"] = events[index].getMedicationName();
        eventObject["timestamp"] = events[index].getTimestamp();
        eventObject["status"] = eventStatusToString(events[index].getStatus());
    }

    return writeConfigDocument(doc);
}

bool StorageManager::clearAcknowledgedEvents(const int eventIds[], int count) {
    JsonDocument doc;
    if (!readConfigDocument(doc)) {
        return true;
    }

    JsonArray eventsArray = doc["events"].as<JsonArray>();
    if (eventsArray.isNull()) {
        return true;
    }

    for (int index = static_cast<int>(eventsArray.size()) - 1; index >= 0; index--) {
        int eventId = eventsArray[index]["id"] | 0;
        if (containsEventId(eventIds, count, eventId)) {
            eventsArray.remove(index);
        }
    }

    return writeConfigDocument(doc);
}

String StorageManager::getConfigFilePath() const {
    return configFilePath;
}
