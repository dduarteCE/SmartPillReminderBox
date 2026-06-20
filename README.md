# Smart Pill Reminder Box (MediMinder)

Smart Pill Reminder Box, presented as **MediMinder**, is an academic hardware/software prototype designed to help older adults follow medication routines with support from a caregiver. The system combines a physical pill organizer, an ESP32-based controller, drawer sensors, visual and audible reminders, and a mobile application for configuration and feedback.

The prototype is intended for **older adults with mild to moderate cognitive impairment** who may need assistance remembering which medication to take and when to take it. It does not replace medical supervision or professional caregiving; instead, it demonstrates how an embedded device can guide a user through a scheduled medication interaction while keeping the user **independent but supervised**: the person uses the box on their own, while a caregiver configures and monitors it remotely through the app.

---

## Project Intent

The goal of this project is to build a functional reminder system that connects a simple caregiver-facing mobile app with a physical pill reminder box.

The system focuses on three main ideas:

- Helping the user identify the correct drawer at the scheduled time.
- Detecting whether the drawer was opened and closed.
- Sending feedback to the caregiver app about the result of the interaction.

The prototype confirms drawer interaction. It does not confirm that the medication was swallowed.

---

## Main Functionality

The Smart Pill Reminder Box supports the following functionality:

- Configure medication reminders from a mobile app.
- Assign a medication to a physical drawer.
- Let the caregiver set the quantity of pills loaded into each drawer.
- Support multiple schedules for the same drawer.
- Activate a multimodal reminder (light, on-screen message, and sound) when the scheduled time arrives.
- Indicate the correct drawer by changing the color of its LED.
- Sound a buzzer during an active reminder.
- Display the medication name and target drawer on an LCD screen.
- Detect drawer opening and closing using magnetic reed switches.
- Notify the mobile app when the interaction is completed or missed.
- Notify the caregiver when a drawer runs out of pills.
- Store configuration locally so the prototype can continue working after restart.

---

## System Overview

The project uses a direct connection between the mobile app and the ESP32 device over WiFi.

```text
Mobile App  <---- HTTP + WebSocket over WiFi ---->  ESP32 Firmware  ---->  Hardware
```

There is no backend server, external database, or MQTT broker in the current prototype scope.

---

## Major Components

### Mobile App

The mobile app is used by the caregiver to configure and monitor the pill reminder box.

Its main responsibilities are:

- Register or edit medication information.
- Assign medications to drawers.
- Configure reminder schedules.
- Set the number of pills loaded in each drawer.
- Send configuration data to the ESP32.
- Receive feedback events from the ESP32.
- Display alerts or status updates to the caregiver, including low-pill notifications.
- Store local app data, such as schedules and interaction history.

### ESP32 Firmware

The ESP32 is the central controller of the physical pill box.

Its main responsibilities are:

- Receive configuration from the mobile app.
- Store drawer and schedule configuration locally.
- Check reminder times.
- Trigger the multimodal reminder: change the assigned drawer LED color, sound the buzzer, and show LCD messages.
- Read drawer sensors.
- Determine whether the drawer interaction was completed or missed.
- Track remaining pills per drawer and report when a drawer is empty.
- Send feedback events to the mobile app.

### Physical Hardware

The physical prototype includes:

- ESP32 microcontroller.
- Transparent acrylic drawer compartments, so the dose stays visible at a glance.
- Magnetic reed switches for drawer detection.
- LEDs for visual guidance.
- Buzzer or small speaker for audible reminders.
- LCD screen for text instructions.
- Resistors, wiring, and supporting electronic components.

---

## Communication

The system uses two communication mechanisms:

```text
HTTP      -> mobile app sends configuration to the ESP32
WebSocket -> ESP32 sends feedback events to the mobile app
```

HTTP is used for caregiver actions such as sending drawer assignments and reminder schedules.

WebSocket is used because some information is generated later by the ESP32, after physical interaction occurs. For example, the ESP32 may need to notify the app that a drawer was opened, closed, completed, or that the reminder timed out.

This avoids the need for constant polling and avoids adding an MQTT broker or backend server.

---

## Example Interaction

A typical interaction works like this:

1. The caregiver configures a reminder in the mobile app.
2. The app sends the drawer number, medication name, pill quantity, and schedule to the ESP32.
3. The ESP32 stores the configuration.
4. At the scheduled time, the ESP32 activates the reminder.
5. The assigned drawer LED changes color, the buzzer sounds, and the LCD shows the medication and the drawer to open.
6. The user opens the indicated drawer.
7. The user closes the drawer again.
8. The ESP32 marks the interaction as completed.
9. The ESP32 sends a feedback event to the mobile app.
10. The caregiver app displays or stores the result.

If the drawer is not opened and closed within the allowed time window, the ESP32 reports the reminder as missed.

---

## Prototype Scope

This project is a functional academic prototype. It is designed to demonstrate the main behavior of a smart medication reminder system, not to operate as a certified medical product.

The prototype includes:

- One ESP32-controlled pill reminder box.
- One caregiver-facing mobile app.
- Local WiFi communication.
- Local configuration and feedback handling.
- Drawer-based interaction confirmation.

The prototype does not include:

- Medical certification.
- Cloud synchronization.
- Backend user accounts.
- Remote access outside the local network.
- Verification that the medication was actually swallowed.
- Advanced security, authentication, or encryption features.
- Multiple devices managed through a central server.

---

## Limitations

The system has several intentional limitations:

- **Main limitation:** it only confirms that the drawer was opened and closed. It cannot guarantee that the medication was actually ingested.
- The mobile app and ESP32 must be connected to the same WiFi network.
- WebSocket feedback assumes the app remains open during the prototype demonstration.
- The system is designed for a simple prototype scenario, not for large-scale deployment.
- The local storage approach is simple and not intended for long-term medical record management.

---

## Intended Use

This prototype is intended for academic demonstration and design validation. It shows how embedded hardware, sensors, actuators, and a mobile interface can work together to support medication reminders and caregiver feedback.

The main value of the project is the integration of physical interaction with software feedback: the user receives guidance from the device, and the caregiver receives information about whether the expected drawer interaction occurred.

---

## Repository Structure

```text
SmartPillReminderBox/
├── context/         # Implementation context notes
├── docs/            # Documentation, design PDFs, presentation, demo video
├── firmware/esp32/  # ESP32 firmware (PlatformIO)
├── hardware/        # Components, enclosure, and wiring notes
├── mobile_app/      # Caregiver mobile app (Flutter)
└── tests/           # Firmware and integration tests
```

---

## Documentation

Additional project documentation is available in the [`docs/`](docs/) folder:

- [`commit_workflow.md`](docs/commit_workflow.md) — Git commit and branch workflow.
- [`esp32_api_contract.md`](docs/esp32_api_contract.md) — HTTP and WebSocket API contract for the ESP32.
- `SmartPillReminderBox_DesignDocument.pdf` — System and object-oriented design.
- `SmartPillReminderBox_AttributesDocument.pdf` — Class and attribute specification.
- `Presentation.7z` — Final presentation slides (English).
- `video.mp4` — Prototype demonstration video.