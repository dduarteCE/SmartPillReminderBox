# Commit & Branch Workflow

> **Purpose**  
> Keep a clean and understandable Git history using a lightweight Conventional Commits style and a simple branch workflow for the Smart Pill Reminder Box project.

---

## Branch Model

The repository will use three main branch tracks:

- `master` → release-quality code only. It should contain tagged versions such as `v1.0.0`.
- `develop` → integration branch for completed work during the current sprint or implementation phase.
- `feature/*` → short-lived branches for individual features, fixes, or project tasks.

### Branch Naming

Use short and descriptive branch names.

```bash
feature/esp32-reminder-controller
feature/websocket-feedback
feature/mobile-schedule-screen
feature/lcd-display-messages
feature/reed-switch-reading
feature/storage-json-config
```

For urgent fixes after a release, use:

```bash
hotfix/<short-topic>
```

Example:

```bash
hotfix/fix-wrong-drawer-event
```

---

## Commit Message Format

We follow a compact Conventional Commits style:

```text
<type>(<scope>): <subject>

<body>

<footer>
```

### Required Fields

- `type`: required
- `scope`: optional, but recommended
- `subject`: required

The subject should be written in English, use imperative style, stay under 72 characters, and not end with a period.

---

## Commit Types

Use the following commit types:

- `feat`: new functionality
- `fix`: bug fix
- `docs`: documentation-only changes
- `style`: formatting, whitespace, or code style changes
- `refactor`: internal code restructuring without changing behavior
- `perf`: performance improvements
- `test`: tests or test-related changes
- `build`: build system, dependencies, or tooling
- `ci`: CI/CD configuration
- `chore`: maintenance tasks
- `revert`: revert a previous commit

---

## Suggested Scopes for This Project

Use scopes that match the project structure and responsibilities:

- `firmware`: general ESP32 firmware changes
- `mobile`: mobile app changes
- `hardware`: wiring, components, enclosure, or physical design notes
- `docs`: documentation files
- `readme`: README changes
- `websocket`: WebSocket feedback communication
- `http`: HTTP configuration endpoints
- `storage`: JSON or persistent storage logic
- `reminder`: reminder state machine and schedule execution
- `drawer`: drawer model, reed switch, and drawer interaction logic
- `lcd`: LCD display behavior
- `buzzer`: buzzer behavior
- `led`: LED behavior
- `config`: system constants, pin configuration, or app/device configuration
- `ui`: mobile app screens and visual interface
- `models`: shared or app-side data models
- `tests`: test files or manual verification notes

---

## Examples

### Project structure

```text
chore: setup initial project structure
```

### README update

```text
docs(readme): describe project intent and prototype limitations
```

### ESP32 reminder logic

```text
feat(reminder): add reminder state machine
```

### Drawer interaction

```text
feat(drawer): detect drawer open and close cycle
```

### WebSocket feedback

```text
feat(websocket): publish dose events to mobile app
```

### HTTP configuration

```text
feat(http): add endpoint to receive drawer schedules
```

### ESP32 storage

```text
feat(storage): persist ESP32 configuration in JSON
```

### Mobile app storage

```text
feat(mobile): store schedules and dose records locally
```

### LCD update

```text
feat(lcd): show medicine name and drawer number
```

### Bug fix

```text
fix(reminder): prevent repeated trigger for same schedule
```

### Refactor

```text
refactor(firmware): move pin constants into PinConfig
```

### Documentation

```text
docs: add class design details
```

### Breaking change

```text
feat!(config): change schedule JSON structure

BREAKING CHANGE: Schedule configuration now requires scheduleId and daysOfWeek.
Update the mobile app before sending new configurations to the ESP32.
```

---

## Daily Workflow

### 1. Start a feature branch

```bash
git checkout develop
git pull
git checkout -b feature/<short-topic>
```

Example:

```bash
git checkout -b feature/websocket-feedback
```

---

### 2. Commit small logical changes

Each commit should represent one clear change.

Good:

```text
feat(drawer): add reed switch read method
feat(led): add blink behavior for active drawer
feat(reminder): detect completed drawer interaction
```

Avoid:

```text
feat: add everything
```

If a task touches multiple areas, prefer separate commits when possible.

Example:

```text
feat(firmware): add schedule model
feat(storage): save schedules to JSON
feat(http): receive schedule configuration
```

---

### 3. Open a pull request into `develop`

Feature branches should be merged into `develop`.

The PR title should follow the same format as the commit style.

Example:

```text
feat(websocket): add ESP32 feedback event channel
```

### Minimal PR Checklist

- [ ] Code compiles
- [ ] Feature was manually tested when applicable
- [ ] No unrelated files were changed
- [ ] Documentation updated if behavior changed
- [ ] JSON format changes are clearly explained
- [ ] Breaking changes are marked clearly

Use **Squash & Merge** when merging into `develop` so the history stays clean.

---

## Release Workflow

When the current version is ready:

```bash
git checkout master
git pull
git merge develop
git tag -a v1.0.0 -m "Release v1.0.0"
git push
git push --tags
```

The `master` branch should only contain stable, demo-ready versions.

---

## Hotfix Workflow

Use hotfix branches only for urgent fixes to released code.

```bash
git checkout master
git pull
git checkout -b hotfix/<short-topic>
```

After fixing:

1. Merge the hotfix into `master`.
2. Tag a new patch version.
3. Merge the hotfix back into `develop`.

Example:

```bash
git tag -a v1.0.1 -m "Release v1.0.1"
git push --tags
```

---

## Project-Specific Notes

### Firmware commits

Use firmware-related scopes when modifying ESP32 code.

Examples:

```text
feat(firmware): initialize device controller
feat(reminder): add active reminder timeout
fix(drawer): correct reed switch closed-state reading
```

### Mobile app commits

Use mobile-related scopes when modifying the caregiver app.

Examples:

```text
feat(ui): add schedule creation screen
feat(mobile): save dose records to local JSON
fix(ui): show missed dose alert correctly
```

### Communication commits

Use `http` for configuration communication and `websocket` for feedback events.

Examples:

```text
feat(http): receive drawer configuration from app
feat(websocket): send dose completed event
fix(websocket): resend pending events after reconnect
```

### Storage commits

Use `storage` when modifying JSON persistence.

Examples:

```text
feat(storage): load ESP32 config from LittleFS
feat(storage): save mobile app data to JSON
fix(storage): preserve schedules after app restart
```

### Hardware documentation commits

Use `hardware` for wiring, component, or enclosure notes.

Examples:

```text
docs(hardware): add reed switch wiring notes
docs(hardware): document LED resistor selection
```

---

## TL;DR

- Work on `feature/*` branches.
- Merge completed work into `develop`.
- Release stable versions from `develop` into `master`.
- Tag releases with `vX.Y.Z`.
- Use English Conventional-style commits.
- Keep commits small and focused.
- Use scopes that match the project: `firmware`, `mobile`, `websocket`, `http`, `storage`, `reminder`, `drawer`, `lcd`, `buzzer`, `led`, `hardware`, and `docs`.