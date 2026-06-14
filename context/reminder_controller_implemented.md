# Reminder Controller Implemented

This note summarizes the current ReminderController implementation in the ESP32 firmware skeleton.

## What is working

- `ReminderController` stores schedules in a fixed-size array.
- `checkSchedules(...)` scans schedules and starts a reminder when a schedule matches the current `DateTime`.
- Duplicate triggering for the same schedule occurrence is prevented using:
  - `lastTriggeredScheduleId`
  - `lastTriggeredHour`
  - `lastTriggeredMinute`
  - `lastTriggeredDate`
- `startReminder(...)` only starts a reminder when the controller is `Idle`.
- The reminder lifecycle now supports these transitions:
  - `REMINDER_STARTED`
  - `DRAWER_OPENED`
  - `DRAWER_CLOSED`
  - `DOSE_COMPLETED`
  - `DOSE_MISSED`
- `ReminderController` receives a `DrawerManager*` from `DeviceController` and uses it to:
  - check whether the target drawer is open
  - get the drawer medication name for generated events
- `updateReminderState(...)` now:
  - checks for drawer-open transitions
  - checks for drawer-close transitions
  - checks reminder timeout
- `buildEvent(...)` centralizes `DoseEvent` creation.
- `scheduledTime` is stored separately from event `timestamp`:
  - `scheduledTime` comes from the reminder start minute
  - `timestamp` comes from the actual event time passed into the controller
- `DeviceController::loop()` now passes the current `DateTime` into:
  - `checkSchedules(...)`
  - `update(...)`
- Generated events are queued in `pendingEvents` and consumed later by `DeviceController`.

## Current design boundaries

- `ReminderController` owns reminder state and event generation.
- `DeviceController` owns coordination and passes current time into the controller.
- `ReminderController` does not fetch time directly from `ClockModule`.
- `ReminderController` does not publish WebSocket events directly.
