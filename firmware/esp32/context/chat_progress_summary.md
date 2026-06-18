# Chat Progress Summary

This note summarizes the main project work completed during this chat session.

## StorageManager completed

- Implemented `StorageManager` with LittleFS-backed JSON persistence.
- Added shared config document helpers to:
  - read `config.json`
  - write `config.json`
- Implemented persistence for:
  - drawers
  - schedules
  - unacknowledged dose events
- Implemented event acknowledgement cleanup through stored event ID removal.
- Added ArduinoJson to `platformio.ini`.

## Model support added for persistence

- Added read-only accessors to `Schedule` for:
  - time count
  - individual times
  - day count
  - individual days of week
- Added read-only accessors to `DoseEvent` for:
  - schedule ID
  - scheduled time
  - drawer ID
  - medication name
  - timestamp
  - status

## DeviceController integration completed

- Connected `DeviceController` so successful drawer config changes persist immediately.
- Connected `DeviceController` so schedule create/update/delete persists immediately.
- Added in-memory tracking for unacknowledged `DoseEvent`s inside `DeviceController`.
- Connected reminder event publishing so emitted events are:
  - broadcast through `WebSocketService`
  - stored as unacknowledged events for recovery
- Added event acknowledgement support in `DeviceController`.
- Restored pending events from storage during startup.
- Resumed `ReminderController` event IDs from stored events to avoid ID collisions after restart.

## WebServerController API layer implemented

- Replaced the old canned JSON responses with contract-shaped handlers matching:
  - `GET /api/health`
  - `GET /api/drawers`
  - `GET /api/schedules`
  - `GET /api/events`
  - `GET /api/time`
  - `PUT /api/drawers/{drawerId}`
  - `POST /api/schedules`
  - `PUT /api/schedules/{scheduleId}`
  - `DELETE /api/schedules/{scheduleId}`
  - `PUT /api/time`
  - `POST /api/events/ack`
- Added request parsing and validation for:
  - drawer updates
  - schedule create/update
  - time updates
  - event acknowledgements
- Added real `WebServer` transport wiring:
  - static route registration in `begin()`
  - dynamic route dispatch for drawer/schedule IDs
  - JSON response sending through `server.send(...)`
  - live request pumping through `handleClient()`

## Shared helper cleanup completed

- Extracted shared dose-event enum/string conversion helpers into:
  - `include/utils/DoseEventJson.h`
  - `src/utils/DoseEventJson.cpp`
- Extracted shared date/time validation helpers into:
  - `include/utils/DateTimeValidation.h`
  - `src/utils/DateTimeValidation.cpp`
- Updated both `StorageManager` and `WebServerController` to use the shared helpers.

## Validation completed

- Re-ran `pio run` after:
  - storage implementation
  - controller persistence integration
  - helper extraction
  - HTTP server transport wiring
- Current firmware state compiles successfully in PlatformIO.

## Commits and branches created during this chat

- Pushed storage persistence work.
- Pushed controller persistence integration work.
- Merged both into `develop`.
- HTTP API work is currently on branch:
  - `feature/http-api-integration`
