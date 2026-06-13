# Smart Pill Reminder Box — Codex Implementation Context

## Project Intent

Smart Pill Reminder Box is a functional prototype for helping an older adult follow medication schedules. The system combines a mobile/web app with an ESP32-based physical pill box. The app is used by a caregiver to configure medication reminders. The ESP32 executes the reminders locally using LEDs, a buzzer, an LCD screen, and magnetic reed switches installed in the drawers.

The goal is a working prototype, not a scalable production system. Avoid unnecessary backend/server complexity.

---

## Final Architecture Decision

The system has only two software deployment targets:

```text
Mobile/Web App  <---- HTTP + WebSocket over WiFi ---->  ESP32 Firmware
                                                        |
                                                        v
                                                Physical Hardware
```

There is no backend server and no MQTT broker.

### Communication split

- **HTTP** is used for configuration, simple queries, and recovery after reconnect.
- **WebSocket** is used for ESP32-generated feedback events in real time.

The app should not continuously query the ESP32 for runtime reminder state. Reminder state changes are reported through WebSocket events.

---

## Removed / Avoided Complexity

The original class diagram included server-like layering such as domain model, communication layer, and device services. For implementation, these are simplified.

Avoid:

- Backend server
- MQTT broker
- MQTT client logic
- Full domain layer inside the ESP32
- Polling all day
- App-side reminder execution logic
- Caregiver cancellation of active reminders
- Large DTO/message-format classes unless needed by code

The ESP32 owns reminder execution. The app owns configuration and history.

---

## Deployment of Classes

### ESP32 Firmware Classes

These classes live in the ESP32 firmware:

```text
DeviceController
WebServerController
WebSocketService
StorageManager
ReminderController
DrawerManager
Drawer
Schedule
DoseEvent
LED
ReedSwitch
Buzzer
LCDScreen
ClockModule
```

### Mobile/Web App Classes

These classes live in the app:

```text
MobileApp
ESP32Client
Patient
Medication
DrawerConfig
Schedule
DoseRecord
AlertNotification
LocalStorageService
NotificationService
```

### Shared Concepts

Some concepts exist on both sides but do not need identical implementations:

- `Schedule`
- `Drawer` / `DrawerConfig`
- `DoseEvent` / `DoseRecord`

The ESP32 version should be minimal and execution-focused. The app version can be richer and user-facing.


---

## Final Class Attributes and Functions

This section lists the expected attributes and functions for the main classes. Keep the implementation prototype-focused. Do not add backend-like abstractions unless they are required by the current scope.

### ESP32 Firmware Classes

#### `DeviceController`

Main coordinator of the ESP32 firmware.

Attributes:

```text
reminderController

drawerManager
webServerController
webSocketService
storageManager
lcdScreen
buzzer
clockModule
```

Functions:

```text
setup()
loop()
applyDrawerConfig(drawerId, medicationName, enabled)
applySchedule(schedule)
removeSchedule(scheduleId)
onReminderEvent(event)
publishDoseEvent(event)
loadStoredConfiguration()
```

Notes:

- Coordinates the system but does not implement all logic directly.
- Does not expose caregiver reminder cancellation unless the scope changes.
- Delegates HTTP handling to `WebServerController` and real-time feedback to `WebSocketService`.

---

#### `WebServerController`

Handles HTTP requests from the mobile/web app.

Attributes:

```text
server

deviceController
```

Functions:

```text
begin()
handleClient()
handleGetHealth()
handleGetDrawers()
handleGetSchedules()
handleGetEvents()
handleGetTime()
handleUpdateDrawer(drawerId)
handleCreateSchedule()
handleUpdateSchedule(scheduleId)
handleDeleteSchedule(scheduleId)
handleSetTime()
handleAcknowledgeEvents()
```

Notes:

- HTTP is used for configuration, simple queries, time setup, and event recovery.
- Do not add `GET /api/status` for live reminder state. Live state is reported by WebSocket events.

---

#### `WebSocketService`

Sends ESP32-generated feedback events to the app.

Attributes:

```text
webSocketServer
connectedClientsCount
```

Functions:

```text
begin()
handleWebSocket()
sendEvent(event)
sendReminderStarted(event)
sendDrawerOpened(event)
sendDrawerClosed(event)
sendDoseCompleted(event)
sendDoseMissed(event)
hasConnectedClient()
```

Notes:

- Does not decide reminder behavior.
- Only sends events produced by `ReminderController` / `DeviceController`.
- Supported event types: `REMINDER_STARTED`, `DRAWER_OPENED`, `DRAWER_CLOSED`, `DOSE_COMPLETED`, `DOSE_MISSED`.

---

#### `StorageManager`

Handles ESP32 persistent storage.

Attributes:

```text
configFilePath
```

Functions:

```text
begin()
loadDrawers()
saveDrawers(drawers)
loadSchedules()
saveSchedules(schedules)
loadEvents()
saveEvents(events)
clearAcknowledgedEvents(eventIds)
```

Notes:

- Recommended implementation: LittleFS with JSON.
- Stores drawers, schedules, and unacknowledged events.
- Runtime state such as current reminder and LED timing stays in RAM.

---

#### `ReminderController`

Controls schedule checking and reminder execution.

Attributes:

```text
schedules[]
pendingReminders[]
currentSchedule
currentScheduledTime
state
reminderStartTime
interactionWindowMinutes
hasDrawerOpened
```

Functions:

```text
setSchedules(schedules)
addSchedule(schedule)
updateSchedule(schedule)
removeSchedule(scheduleId)
checkSchedules(currentDateTime)
startReminder(schedule, scheduledTime)
updateReminderState()
markDrawerOpened()
markDrawerClosed()
markDoseCompleted()
markDoseMissed()
resetCurrentReminder()
```

Notes:

- Owns reminder timing and state-machine behavior.
- A schedule may contain multiple daily times through `times[]`.
- Multiple schedules may reference the same drawer.
- Completion means the correct drawer was opened and closed; it does not confirm medication ingestion.

---

#### `DrawerManager`

Manages all physical drawers.

Attributes:

```text
drawers[]
```

Functions:

```text
setDrawers(drawers)
getDrawer(drawerId)
configureDrawer(drawerId, medicationName, enabled)
highlightDrawer(drawerId)
stopHighlight(drawerId)
turnOffAllDrawers()
isDrawerOpen(drawerId)
readDrawerState(drawerId)
```

Notes:

- `getDrawer(drawerId)` is kept because the ESP32 needs drawer data for LCD messages, LED control, reed switch reading, and event generation.
- Do not use `selectDrawer(id)`. The active schedule already identifies the drawer through `schedule.drawerId`.

---

#### `Drawer`

Represents one physical drawer/compartment in the ESP32 firmware.

Attributes:

```text
id
medicationName
enabled
led
reedSwitch
state
```

Functions:

```text
isOpen()
highlight()
stopHighlight()
setMedicationName(medicationName)
setEnabled(enabled)
```

Notes:

- The ESP32 `Drawer` includes hardware references.
- The drawer does not own schedules. `Schedule.drawerId` links schedules to drawers.

---

#### `Schedule`

Represents a reminder configuration stored on the ESP32 and also conceptually in the app.

Attributes:

```text
id
drawerId
times[]        // each item has hour and minute
daysOfWeek[]
enabled
```

Example:

```json
{
  "id": 1,
  "drawerId": 1,
  "times": [
    { "hour": 8, "minute": 0 },
    { "hour": 20, "minute": 30 }
  ],
  "daysOfWeek": ["MON", "TUE", "WED", "THU", "FRI"],
  "enabled": true
}
```

Functions:

```text
isEnabled()
shouldTrigger(currentDateTime)
containsTime(hour, minute)
```

Notes:

- One schedule can represent multiple daily occurrences.
- The event must include `scheduledTime` so the app knows which occurrence triggered.

---

#### `DoseEvent`

Represents an event generated by the ESP32.

Attributes:

```text
id
type
scheduleId
scheduledTime
drawerId
medicationName
timestamp
status
```

Functions:

```text
toJson()
```

Valid event types:

```text
REMINDER_STARTED
DRAWER_OPENED
DRAWER_CLOSED
DOSE_COMPLETED
DOSE_MISSED
```

Valid final statuses:

```text
TAKEN
MISSED
```

Notes:

- `TAKEN` means drawer interaction completed, not confirmed ingestion.
- Events are sent through WebSocket and stored until acknowledged by the app.

---

#### Hardware Abstraction Classes

`LED`

Attributes:

```text
pin
isBlinking
lastToggleTime
```

Functions:

```text
begin()
turnOn()
turnOff()
blink()
update()
```

`ReedSwitch`

Attributes:

```text
pin
lastState
```

Functions:

```text
begin()
isOpen()
isClosed()
readState()
```

`Buzzer`

Attributes:

```text
pin
active
```

Functions:

```text
begin()
activate()
deactivate()
```

`LCDScreen`

Attributes:

```text
address
columns
rows
```

Functions:

```text
begin()
clear()
showReminder(medicationName, drawerId)
showMessage(message)
showDoseConfirmed()
showDoseMissed()
```

`ClockModule`

Attributes:

```text
currentDate
currentTime
dayOfWeek
```

Functions:

```text
setTime(currentDate, currentTime, dayOfWeek)
getCurrentDateTime()
getDayOfWeek()
```

---

### Mobile/Web App Classes

#### `MobileApp`

Coordinates the app UI, local storage, and ESP32 communication.

Attributes:

```text
esp32Client
localStorageService
notificationService
patients[]
medications[]
drawers[]
schedules[]
doseRecords[]
```

Functions:

```text
loadLocalData()
saveLocalData()
configureDrawer(drawerConfig)
createSchedule(schedule)
updateSchedule(schedule)
deleteSchedule(scheduleId)
connectToDevice(ipAddress)
handleDoseEvent(event)
showAlert(notification)
```

---

#### `ESP32Client`

Handles HTTP and WebSocket communication with the ESP32.

Attributes:

```text
baseUrl
webSocketUrl
isConnected
```

Functions:

```text
getHealth()
getDrawers()
getSchedules()
getEvents()
getTime()
updateDrawer(drawerConfig)
createSchedule(schedule)
updateSchedule(schedule)
deleteSchedule(scheduleId)
setTime(currentDate, currentTime, dayOfWeek)
acknowledgeEvents(eventIds)
connectWebSocket()
disconnectWebSocket()
onWebSocketEvent(event)
```

Notes:

- HTTP sends configuration and recovery requests.
- WebSocket receives live events.

---

#### `LocalStorageService`

Stores app-side data in local JSON.

Attributes:

```text
storagePath
```

Functions:

```text
loadData()
saveData(data)
saveDrawerConfig(drawerConfig)
saveSchedule(schedule)
removeSchedule(scheduleId)
saveDoseRecord(doseRecord)
```

---

#### `NotificationService`

Creates caregiver-facing alerts from dose records or received events.

Attributes:

```text
notifications[]
```

Functions:

```text
createNotification(type, message)
markAsRead(notificationId)
getUnreadNotifications()
```

---

#### App-Side Models

`Patient`

Attributes:

```text
id
name
```

`Medication`

Attributes:

```text
id
name
dosage
instructions
```

`DrawerConfig`

Attributes:

```text
drawerId
medicationId
medicationName
enabled
```

`Schedule`

Attributes:

```text
id
drawerId
times[]
daysOfWeek[]
enabled
```

`DoseRecord`

Attributes:

```text
id
scheduleId
scheduledTime
drawerId
medicationName
timestamp
status
```

`AlertNotification`

Attributes:

```text
id
type
message
timestamp
isRead
```


---

## Data Ownership

### Mobile/Web App Storage

The app stores the complete user-facing data, likely in a local JSON file for the prototype:

```text
Patient
Medication
DrawerConfig
Schedule
DoseRecord
AlertNotification
```

The app is the main source of truth for caregiver-facing configuration and history.

### ESP32 Storage

The ESP32 stores only what it needs to operate independently:

```text
drawers[]
schedules[]
recent/unacknowledged DoseEvents
```

Recommended ESP32 persistent storage: LittleFS JSON file.

ESP32 RAM stores runtime-only data:

```text
current reminder
current state
pending reminder processing
live drawer state
LED/buzzer timing
```

---

## Schedule Modeling

A drawer can have multiple schedules, and a schedule can contain multiple times per day.

Use a `times` array instead of a single `hour` and `minute` field.

Example:

```json
{
  "id": 1,
  "drawerId": 1,
  "times": [
    { "hour": 8, "minute": 0 },
    { "hour": 20, "minute": 30 }
  ],
  "daysOfWeek": ["MON", "TUE", "WED", "THU", "FRI"],
  "enabled": true
}
```

Multiple schedules may reference the same `drawerId`.

---

## Runtime Reminder Behavior

Align the firmware behavior with the existing state machine in the design document:

```text
IDLE
  -> scheduled time reached
REMINDER_ACTIVE
  -> correct drawer opened
DRAWER_OPEN / WAITING_FOR_CLOSE
  -> drawer closed again
INTERACTION_COMPLETE
  -> send feedback event
SENDING_FEEDBACK
  -> return to IDLE
```

Timeout path:

```text
REMINDER_ACTIVE or WAITING_FOR_CLOSE
  -> time window expired
TIMEOUT
  -> send missed-dose feedback event
SENDING_FEEDBACK
  -> return to IDLE
```

The system confirms drawer interaction, not medication ingestion. A completed dose means the correct drawer was opened and closed.

---

## WebSocket Feedback Events

WebSocket is used for real-time ESP32-generated feedback.

Endpoint:

```text
ws://<esp32-ip>/ws/events
```

Events to support:

```text
REMINDER_STARTED
DRAWER_OPENED
DRAWER_CLOSED
DOSE_COMPLETED
DOSE_MISSED
```

Do not include `REMINDER_CANCELLED` unless the product explicitly adds caregiver cancellation later.

Every event should include at least:

```json
{
  "id": 204,
  "type": "DOSE_COMPLETED",
  "scheduleId": 1,
  "scheduledTime": "08:00",
  "drawerId": 1,
  "medicationName": "Aspirin",
  "timestamp": "2026-06-13T08:02:00",
  "status": "TAKEN"
}
```

For `DOSE_COMPLETED`, `TAKEN` means drawer interaction completed, not confirmed ingestion.

---

## HTTP API Contract Summary

HTTP is for configuration, time setup, health checks, and event recovery after reconnect.

### GET

```http
GET /api/health
GET /api/drawers
GET /api/schedules
GET /api/events
GET /api/time
```

Notes:

- `GET /api/events` returns ESP32-generated events not yet acknowledged by the app.
- It is mainly for recovery after app close, WiFi disconnection, or WebSocket interruption.
- Do not implement `GET /api/status` for live reminder state; WebSocket handles runtime state updates.

### PUT

```http
PUT /api/drawers/{drawerId}
PUT /api/schedules/{scheduleId}
PUT /api/time
```

Use `PUT` to update/replace known resources.

### POST

```http
POST /api/schedules
POST /api/events/ack
```

Use `POST /api/schedules` to create a new schedule. Use `POST /api/events/ack` after the app receives and stores events.

### DELETE

```http
DELETE /api/schedules/{scheduleId}
```

Schedules can be deleted. Drawers are physical compartments, so they should generally be updated, disabled, or cleared instead of deleted.

### Not included

Do not implement:

```http
POST /api/reminder/cancel
GET /api/status
```

unless the scope changes.

---

## HTTP Contract Style

The API contract should be explicit and atomic:

- Separate endpoints for drawers and schedules.
- Do not require sending the entire configuration every time one schedule changes.
- Request names should be clear, such as `getDrawers`, `createSchedule`, `updateSchedule`, and `acknowledgeEvents`.
- Response bodies should always include `success` and either useful data or an `error` + `message`.

Example success response:

```json
{
  "success": true,
  "message": "Schedule updated",
  "schedule": {
    "id": 2,
    "drawerId": 1,
    "times": [
      { "hour": 8, "minute": 0 },
      { "hour": 20, "minute": 30 }
    ],
    "daysOfWeek": ["MON", "TUE", "WED", "THU", "FRI"],
    "enabled": true
  }
}
```

Example error response:

```json
{
  "success": false,
  "error": "SCHEDULE_NOT_FOUND",
  "message": "Schedule 2 does not exist"
}
```

---

## Repository Structure Proposal

```text
smart-pill-reminder-box/
├── README.md
├── docs/
├── firmware/
│   └── esp32/
│       └── SmartPillReminderBox/
│           ├── SmartPillReminderBox.ino
│           ├── config/
│           ├── controllers/
│           ├── models/
│           ├── hardware/
│           ├── communication/
│           ├── storage/
│           └── data/
├── mobile-app/
│   ├── src/
│   │   ├── app/
│   │   ├── models/
│   │   ├── services/
│   │   ├── screens/
│   │   └── data/
│   └── assets/
├── hardware/
│   ├── wiring/
│   ├── enclosure/
│   └── components/
└── tests/
```

---

## Commit Workflow Summary

Use lightweight Conventional Commits in English.

Branch model:

```text
master   -> release-quality tagged code
develop  -> integration branch
feature/* -> short-lived work branches
hotfix/*  -> urgent release fixes
```

Suggested scopes:

```text
firmware, mobile, hardware, docs, readme, websocket, http,
storage, reminder, drawer, lcd, buzzer, led, config, ui,
models, tests
```

Examples:

```text
feat(reminder): add reminder state machine
feat(websocket): publish dose completed event
feat(http): add schedule update endpoint
fix(drawer): correct reed switch closed-state reading
docs(readme): describe project intent and limitations
```

---

## Implementation Goal

Build a functional prototype where:

1. The caregiver configures drawer medication and schedules in the app.
2. The app sends drawer and schedule configuration to the ESP32 using HTTP.
3. The ESP32 stores configuration locally.
4. At scheduled times, the ESP32 activates the correct drawer LED, buzzer, and LCD message.
5. The ESP32 detects correct drawer open/close using a reed switch.
6. The ESP32 sends WebSocket feedback events to the app.
7. The app stores those events as dose records.
8. If the app disconnects, it can recover unacknowledged events using `GET /api/events`.

Keep the implementation simple, direct, and prototype-focused.