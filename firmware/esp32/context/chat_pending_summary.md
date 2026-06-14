# Chat Pending Summary

This note tracks the most important work still pending after the changes completed in this chat.

## Networking pending

- `WebServerController` now has a real `WebServer`, but WiFi setup is still missing:
  - no STA/AP mode decision implemented
  - no credentials/config source
  - no connection lifecycle handling
  - no device IP reporting flow
- `WebSocketService` is still stubbed:
  - no real `/ws/events` endpoint
  - no client tracking
  - no event broadcast over an actual socket transport

## HTTP follow-up pending

- The HTTP endpoint logic is implemented, but it still needs real network validation once WiFi is available.
- Consider extracting remaining JSON serialization helpers from `WebServerController` if the file continues to grow:
  - drawer response serialization
  - schedule response serialization
  - event response serialization
- Consider cleaning the schedule create/update handler duplication with a shared helper.

## Clock and display pending

- `ClockModule` is still placeholder-only:
  - no RTC integration
  - no NTP integration
  - no persistence of time across power cycles
- `LCDScreen` is still placeholder-only:
  - no display driver/library integration
  - no real rendering to hardware

## ReminderController follow-up

- Review `ReminderController` for final polish:
  - naming consistency such as `drawer_open`
  - possible `nextEventID` start value change from `0` to `1`
  - optional zero-padding of event times
- Validate that every state transition matches the intended reminder state machine on hardware.

## Hardware pending

- Replace the placeholder `-1` pins for drawers `3` through `7` with real hardware mappings.
- Confirm the final LED and reed switch pin assignments.
- Decide whether all 7 drawers will be active in the first physical prototype or only a subset.

## Validation pending

- Run real end-to-end tests over the network once WiFi setup exists.
- Validate reminder start, drawer open, drawer close, completion, and timeout behavior on hardware.
- Verify that duplicate-trigger prevention works across real loop timing and restart scenarios.
- Validate recovery flows for:
  - restart with stored unacknowledged events
  - HTTP `GET /api/events`
  - HTTP `POST /api/events/ack`
  - reconnect after WebSocket interruption
