# Reminder Controller Pending

This note tracks what is still pending after the current ReminderController phase.

## Still pending inside ReminderController

- Decide whether `nextEventID` should start at `1` instead of `0`.
- Improve formatting consistency:
  - `drawer_open` to `drawerOpen`
  - spacing in `if (...)` statements
- Consider whether `buildEvent(...)` should zero-pad times like `08:00` instead of `8:0`.
- Add a small review pass to confirm every state transition is exactly the intended one.

## Still pending outside ReminderController

- `DeviceController` still does not actively call drawer interaction methods from any external hardware workflow beyond the current reminder loop.
- `StorageManager` is still a stub:
  - no LittleFS
  - no JSON load/save
  - no persisted events
- `WebServerController` is still a stub:
  - no WiFi
  - no real HTTP server
  - no working endpoints
- `WebSocketService` is still a stub:
  - no real WebSocket server
  - no connected-client broadcasting
- `ClockModule` is still a placeholder and does not yet use a real RTC or network time source.
- `LCDScreen` is still a placeholder and does not talk to a real display library.
- End-to-end reminder validation on actual hardware is still pending.

## Good next implementation targets

1. Real storage with LittleFS and config JSON.
2. Real HTTP endpoints for drawers and schedules.
3. Real WebSocket broadcasting for reminder events.
4. Hardware validation for drawer open/close timing and reminder timeout behavior.
