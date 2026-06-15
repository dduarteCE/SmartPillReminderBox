# Chat Pending Summary

This note tracks the work that is still genuinely pending after the changes completed in this chat.

## Hardware validation pending

- Run a real board-level test for the full reminder flow:
  - scheduled reminder starts
  - correct drawer LED blinks
  - buzzer activates
  - LCD shows the expected message
  - drawer open and close complete the dose
  - timeout produces `DOSE_MISSED`
- Confirm the actual LCD I2C address and wiring on the physical module.
- Confirm buzzer polarity and expected active state on the real board.
- Validate reed switch behavior on hardware:
  - correct open and closed readings
  - no inverted state
  - no false triggers

## Pin mapping pending

- Replace the placeholder `-1` pin assignments for drawers `3` through `7` with real GPIO mappings if those drawers are part of the prototype.
- Confirm the final LED and reed switch assignments for every active drawer.
- Decide whether the first physical prototype supports all `7` drawers or only the currently mapped subset.

## Clock follow-up pending

- `ClockModule` is now implemented as a software clock, but it still has these open limitations:
  - no RTC hardware integration
  - no NTP synchronization
  - no persistence across reboot or power loss
- Validate real runtime behavior after calling the time endpoint:
  - time advances correctly
  - midnight rollover updates date
  - day-of-week rollover updates correctly

## WebSocket contract follow-up pending

- `WebSocketService` now uses a real `WebSocketsServer` and broadcasts events, but the transport shape still needs contract alignment.
- Current implementation uses port `81` via `WEBSOCKET_PORT`.
- The contract document describes `ws://<esp32-ip>/ws/events`.
- Decide whether the client should connect to the current port-based server or whether firmware should be adapted to match the documented `/ws/events` endpoint shape more closely.

## Network and API validation pending

- Run real end-to-end tests against the ESP32 access point:
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
- Validate client reconnection behavior for WebSocket event delivery.
- Verify that the mobile or web client can consume the current event payloads without contract drift.

## Storage and recovery validation pending

- Verify that stored drawers reload correctly after restart.
- Verify that stored schedules reload correctly after restart.
- Verify that unacknowledged events reload correctly after restart.
- Validate the recovery path for:
  - restart with pending unacknowledged events
  - `GET /api/events`
  - `POST /api/events/ack`
- Verify duplicate-trigger prevention across restart and real loop timing.

## Optional future improvements

- If the project needs a more production-like time source, add either:
  - RTC module support, or
  - NTP synchronization when networking mode allows it
- If `WebServerController` keeps growing, consider extracting more response-building helpers into separate utility code.
