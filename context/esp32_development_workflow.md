# ESP32 Development Workflow

> **Purpose**
> Define the development workflow for the ESP32 firmware side of the Smart Pill Reminder Box project. This file is intended to guide implementation from scratch while keeping the firmware modular, testable, and aligned with the project design decisions.

---

## 1. Starting Point

The ESP32 side already has the following supporting materials:

- Project structure
- Codex project context
- Commit and branch workflow
- ESP32 HTTP/WebSocket API contract
- Class design notes
- Project README

The ESP32 firmware should be developed gradually. The first objective is not to implement the full product immediately, but to make the firmware compile with the expected class structure and then add functionality layer by layer.

The main firmware entry point should remain simple:

```cpp
#include "controllers/DeviceController.h"

DeviceController device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
```

The `.ino` file should only delegate to `DeviceController`.

---

## 2. Development Strategy

Develop the ESP32 firmware in the following order:

1. Firmware skeleton
2. Hardware abstraction classes
3. Data models
4. Drawer management
5. Persistent storage
6. Reminder state machine
7. WebSocket feedback events
8. HTTP API endpoints
9. Full controller integration
10. End-to-end validation

The key rule is:

> Build and test local reminder behavior before adding HTTP and WebSocket communication.

This prevents communication logic from hiding problems in the reminder state machine or hardware handling.

---

## 3. Phase 1 — Firmware Skeleton

### Goal

Make the ESP32 project compile with the basic class structure.

### Files/classes to create

Controllers:

- `DeviceController`
- `ReminderController`
- `DrawerManager`

Communication:

- `WebServerController`
- `WebSocketService`

Storage:

- `StorageManager`

Models:

- `Drawer`
- `Schedule`
- `DoseEvent`

Hardware abstraction:

- `LED`
- `ReedSwitch`
- `Buzzer`
- `LCDScreen`
- `ClockModule`

At this phase, methods may be empty or return mock values. The goal is only to verify that the project structure compiles.

### Suggested branch

```bash
git checkout develop
git pull
git checkout -b feature/esp32-firmware-skeleton
```

### Suggested commits

```text
chore(firmware): add ESP32 firmware skeleton
chore(firmware): add controller and model skeletons
```

---

## 4. Phase 2 — Pin Configuration and Hardware Wrappers

### Goal

Make each hardware component testable individually.

### Configuration files

Create:

- `PinConfig.h`
- `SystemConfig.h`

Example pin configuration:

```cpp
constexpr int LED_DRAWER_1_PIN = 12;
constexpr int LED_DRAWER_2_PIN = 13;

constexpr int REED_DRAWER_1_PIN = 25;
constexpr int REED_DRAWER_2_PIN = 26;

constexpr int BUZZER_PIN = 27;
```

Actual pin values should be updated according to the final wiring.

### Implementation order

1. `LED`
2. `ReedSwitch`
3. `Buzzer`
4. `LCDScreen`
5. `ClockModule`

### Expected behavior

- `LED` can turn on, turn off, toggle, and blink.
- `ReedSwitch` can detect drawer open/closed state.
- `Buzzer` can activate and deactivate.
- `LCDScreen` can show reminder and status messages.
- `ClockModule` can provide current date/time and day of week.

### Suggested commits

```text
feat(led): add drawer LED control
feat(drawer): add reed switch reading
feat(buzzer): add buzzer control
feat(lcd): add LCD message display
feat(firmware): add clock module
```

---

## 5. Phase 3 — Data Models

### Goal

Define the data structures used by the ESP32 firmware.

### Models

- `Drawer`
- `Schedule`
- `ScheduleTime`
- `DoseEvent`

### Important modeling decision

A schedule can contain multiple times per day because a medication may need to be taken more than once in the same day.

Example:

```cpp
struct ScheduleTime {
    int hour;
    int minute;
};
```

A `Schedule` should reference a drawer by `drawerId` and store one or more `ScheduleTime` entries.

### Suggested `Drawer` attributes

- `id`
- `medicationName`
- `enabled`
- `LED led`
- `ReedSwitch reedSwitch`

### Suggested `Drawer` functions

- `getId()`
- `getMedicationName()`
- `isEnabled()`
- `isOpen()`
- `setMedicationName(...)`
- `setEnabled(...)`
- `highlight()`
- `stopHighlight()`

### Suggested `Schedule` attributes

- `id`
- `drawerId`
- `enabled`
- `times[]`
- `timeCount`
- `daysOfWeek[]`
- `dayCount`

### Suggested `Schedule` functions

- `getId()`
- `getDrawerId()`
- `isEnabled()`
- `matchesCurrentTime(...)`
- `containsDay(...)`
- `containsTime(...)`

### Suggested `DoseEvent` attributes

- `id`
- `type`
- `scheduleId`
- `scheduledTime`
- `drawerId`
- `medicationName`
- `timestamp`
- `status`

### Suggested commits

```text
feat(models): add drawer model
feat(models): add schedule model with multiple daily times
feat(models): add dose event model
```

---

## 6. Phase 4 — DrawerManager

### Goal

Centralize all drawer access and avoid direct pin/hardware access from reminder logic.

### Responsibility

`DrawerManager` owns and manages all `Drawer` objects.

### Suggested functions

```cpp
void setup();
Drawer* getDrawer(int drawerId);
bool isDrawerOpen(int drawerId);
void highlightDrawer(int drawerId);
void stopHighlight(int drawerId);
void stopAllHighlights();
bool configureDrawer(int drawerId, const String& medicationName, bool enabled);
```

### Design rule

`ReminderController` should not directly touch LEDs or reed switch pins. It should ask `DrawerManager` for drawer state and actions.

### Manual tests

- Get drawer by ID.
- Read drawer open/closed state.
- Highlight drawer LED.
- Stop drawer highlight.
- Configure medication name and enabled state.

### Suggested commit

```text
feat(drawer): add drawer manager
```

---

## 7. Phase 5 — StorageManager

### Goal

Save and load ESP32 configuration from persistent flash storage.

### Storage choice

Use:

```text
LittleFS + JSON
```

### File

```text
/config.json
```

### Data stored on ESP32

The ESP32 should persist only the data needed to operate independently:

- Drawers
- Schedules
- Optionally unacknowledged events for reconnection recovery

The ESP32 should not store the full mobile app history.

### Example config

```json
{
  "drawers": [
    {
      "id": 1,
      "medicationName": "Aspirin",
      "enabled": true
    }
  ],
  "schedules": [
    {
      "id": 1,
      "drawerId": 1,
      "times": [
        { "hour": 8, "minute": 0 },
        { "hour": 20, "minute": 0 }
      ],
      "daysOfWeek": ["MON", "TUE", "WED"],
      "enabled": true
    }
  ]
}
```

### Suggested functions

A simple approach is preferred:

```cpp
bool begin();
bool loadConfig(SystemConfigData& config);
bool saveConfig(const SystemConfigData& config);
bool loadUnacknowledgedEvents(...);
bool saveUnacknowledgedEvents(...);
```

The HTTP API may expose atomic endpoints, but internally `StorageManager` may still rewrite the full JSON file when a drawer or schedule changes.

### Suggested commits

```text
feat(storage): initialize LittleFS config storage
feat(storage): load drawer configuration from JSON
feat(storage): load schedules from JSON
feat(storage): persist schedule changes
```

---

## 8. Phase 6 — ReminderController and State Machine

### Goal

Implement the core reminder behavior aligned with the state machine from the design document.

### States

- `IDLE`
- `REMINDER_ACTIVE`
- `DRAWER_OPEN`
- `WAITING_FOR_CLOSE`
- `TIMEOUT`
- `INTERACTION_COMPLETE`
- `SENDING_FEEDBACK`

### Normal flow

```text
IDLE
  scheduled time reached
REMINDER_ACTIVE
  correct drawer opened
DRAWER_OPEN / WAITING_FOR_CLOSE
  drawer closed again
INTERACTION_COMPLETE
  create DOSE_COMPLETED event
SENDING_FEEDBACK
  return to IDLE
```

### Timeout flow

```text
REMINDER_ACTIVE or WAITING_FOR_CLOSE
  time window expired
TIMEOUT
  create DOSE_MISSED event
SENDING_FEEDBACK
  return to IDLE
```

### Responsibility boundary

`ReminderController` should not publish WebSocket events directly. It should produce `DoseEvent` objects or expose pending events to `DeviceController`.

### Suggested functions

```cpp
void setup();
void update();
void addSchedule(const Schedule& schedule);
bool updateSchedule(int scheduleId, const Schedule& schedule);
bool deleteSchedule(int scheduleId);
ReminderState getState() const;
bool hasPendingEvent() const;
DoseEvent popPendingEvent();
```

### Suggested commits

```text
feat(reminder): add reminder state machine
feat(reminder): trigger reminders from schedule times
feat(reminder): detect drawer open close cycle
feat(reminder): generate dose completed event
feat(reminder): generate missed dose event on timeout
```

---

## 9. Phase 7 — WebSocketService

### Goal

Send ESP32-generated feedback events to the mobile/web app in real time.

### WebSocket endpoint

```text
/ws/events
```

### Events sent by ESP32

- `REMINDER_STARTED`
- `DRAWER_OPENED`
- `DRAWER_CLOSED`
- `DOSE_COMPLETED`
- `DOSE_MISSED`

Do not include `REMINDER_CANCELLED`, because caregiver cancellation is not part of the current product scope.

### Suggested functions

```cpp
void begin();
void loop();
void broadcastEvent(const DoseEvent& event);
int connectedClientCount() const;
```

### Design rule

`WebSocketService` should not know reminder logic. It only serializes and sends events.

### Suggested commits

```text
feat(websocket): add ESP32 event WebSocket server
feat(websocket): broadcast reminder feedback events
```

---

## 10. Phase 8 — HTTP API

### Goal

Implement the atomic HTTP contract between the ESP32 and the mobile/web app.

### HTTP is used for

- Configuration
- Time setup
- Event recovery after reconnect
- Event acknowledgement
- Health check

### HTTP is not used for

- Live reminder state polling
- Normal feedback delivery
- Caregiver cancellation

Live feedback is handled through WebSocket.

### Implementation order

1. `GET /api/health`
2. `GET /api/time`
3. `PUT /api/time`
4. `GET /api/drawers`
5. `PUT /api/drawers/{drawerId}`
6. `GET /api/schedules`
7. `POST /api/schedules`
8. `PUT /api/schedules/{scheduleId}`
9. `DELETE /api/schedules/{scheduleId}`
10. `GET /api/events`
11. `POST /api/events/ack`

### Endpoints intentionally excluded

- `GET /api/status`
- `POST /api/reminder/cancel`

### Suggested commits

```text
feat(http): add health endpoint
feat(http): add device time endpoints
feat(http): add drawer configuration endpoints
feat(http): add schedule configuration endpoints
feat(http): add event recovery endpoints
```

---

## 11. Phase 9 — DeviceController Integration

### Goal

Connect all firmware modules through `DeviceController`.

### `DeviceController` coordinates

- `StorageManager`
- `DrawerManager`
- `ReminderController`
- `WebServerController`
- `WebSocketService`
- `LCDScreen`
- `Buzzer`

### Main loop idea

```cpp
void DeviceController::loop() {
    webServer.loop();
    webSocket.loop();

    reminderController.update();

    while (reminderController.hasPendingEvent()) {
        DoseEvent event = reminderController.popPendingEvent();
        webSocket.broadcastEvent(event);
        storageManager.storeUnacknowledgedEvent(event);
    }
}
```

### Integration rules

- HTTP config endpoints update `DrawerManager` and `ReminderController`.
- `StorageManager` persists drawer and schedule changes.
- `ReminderController` executes schedules and generates events.
- `WebSocketService` sends generated events to the app.
- The app acknowledges received events through HTTP.

### Suggested commit

```text
feat(firmware): integrate ESP32 controllers
```

---

## 12. Phase 10 — End-to-End Validation

### Goal

Validate the complete prototype flow.

### Completed dose scenario

```text
1. Configure drawer 1 as Aspirin.
2. Create a schedule for current time + 1 minute.
3. ESP32 saves the configuration.
4. At the scheduled time, ESP32 activates LCD, LED, and buzzer.
5. WebSocket sends REMINDER_STARTED.
6. Open drawer 1.
7. WebSocket sends DRAWER_OPENED.
8. Close drawer 1.
9. ESP32 stops reminder outputs.
10. WebSocket sends DOSE_COMPLETED.
11. App stores the result as a DoseRecord.
12. App acknowledges the event.
```

### Missed dose scenario

```text
1. Configure a schedule.
2. Wait for the scheduled time.
3. Do not open the drawer.
4. Wait until the reminder window expires.
5. ESP32 sends DOSE_MISSED.
6. App stores the result as a DoseRecord.
7. App acknowledges the event.
```

### Suggested commits

```text
test(firmware): add manual ESP32 validation checklist
fix(...): address issues found during integration
```

---

## 13. Suggested Branch Order

Use one feature branch per phase or per small group of related phases.

```text
feature/esp32-firmware-skeleton
feature/esp32-hardware-abstractions
feature/esp32-models
feature/esp32-storage
feature/esp32-reminder-state-machine
feature/esp32-websocket-events
feature/esp32-http-api
feature/esp32-integration
```

Each branch should be merged into `develop` using the project commit workflow.

---

## 14. Recommended Codex Prompt Strategy

Do not ask Codex to implement the whole ESP32 side at once.

Use small, bounded prompts.

Examples:

```text
Implement LED.h and LED.cpp for Arduino ESP32.
Use the existing project structure and PinConfig.h.
Keep the class simple with begin(), turnOn(), turnOff(), toggle(), and blink().
```

```text
Implement ReedSwitch.h and ReedSwitch.cpp.
The class should support INPUT_PULLUP and expose isOpen().
```

```text
Implement the Schedule model.
A schedule must support multiple times per day using ScheduleTime[].
Use fixed-size arrays instead of dynamic allocation.
```

```text
Implement ReminderController using the state machine from codex_project_context.md.
Do not add networking logic here.
ReminderController should only generate DoseEvent objects.
```

This keeps Codex aligned with the architecture and prevents it from inventing extra layers.

---

## 15. Minimum Milestones

### Milestone 1 — Hardware smoke test

- LED blinks.
- Reed switch reads open/closed.
- Buzzer sounds.
- LCD prints message.

### Milestone 2 — Local reminder

- Hardcoded schedule triggers.
- Correct drawer LED blinks.
- Buzzer activates.
- LCD displays medication name and drawer number.
- Drawer open/close completes reminder.
- Timeout generates missed event.

### Milestone 3 — Persistent config

- Config loads from JSON.
- Config survives ESP32 restart.
- Schedules support multiple daily times.

### Milestone 4 — HTTP config

- App/Postman can create and update drawers.
- App/Postman can create, update, and delete schedules.
- ESP32 saves changes.

### Milestone 5 — WebSocket feedback

- App receives `REMINDER_STARTED`.
- App receives `DRAWER_OPENED`.
- App receives `DRAWER_CLOSED`.
- App receives `DOSE_COMPLETED`.
- App receives `DOSE_MISSED`.

### Milestone 6 — Full demo

- Configure schedule from app.
- ESP32 executes reminder.
- Drawer interaction is detected.
- App receives and stores feedback.

---

## 16. Final Implementation Rule

The ESP32 should own the reminder execution.

The mobile/web app should own configuration and history display.

HTTP should be used for atomic configuration and recovery.

WebSocket should be used for ESP32-generated feedback events.

No MQTT broker, backend server, or polling-based live state tracking should be introduced in the ESP32 implementation.
