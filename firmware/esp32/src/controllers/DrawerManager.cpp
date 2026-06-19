#include "controllers/DrawerManager.h"

#include "config/PinConfig.h"

DrawerManager::DrawerManager()
    : drawerCount(MAX_DRAWERS),
      drawerLights(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800),
      highlightedDrawerId(0),
      highlightLightsOn(false),
      lastHighlightToggleMs(0),
      reminderColorIndex(0) {
    reset();
}

void DrawerManager::begin() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].begin();
    }
    drawerLights.begin();
    clearDrawerLights();
}

void DrawerManager::reset() {
    drawers[0] = Drawer(1, "", false, LED_DRAWER_1_PIN, REED_DRAWER_1_PIN);
    drawers[1] = Drawer(2, "", false, LED_DRAWER_2_PIN, REED_DRAWER_2_PIN);
    drawers[2] = Drawer(3, "", false, LED_DRAWER_3_PIN, REED_DRAWER_3_PIN);
    drawers[3] = Drawer(4, "", false, LED_DRAWER_4_PIN, REED_DRAWER_4_PIN);
    drawers[4] = Drawer(5, "", false, LED_DRAWER_5_PIN, REED_DRAWER_5_PIN);
    drawers[5] = Drawer(6, "", false, LED_DRAWER_6_PIN, REED_DRAWER_6_PIN);
    drawers[6] = Drawer(7, "", false, LED_DRAWER_7_PIN, REED_DRAWER_7_PIN);
    drawerCount = MAX_DRAWERS;
    highlightedDrawerId = 0;
    highlightLightsOn = false;
    lastHighlightToggleMs = 0;
    reminderColorIndex = 0;
}

void DrawerManager::setDrawers(const Drawer drawers[], int count) {
    drawerCount = min(count, MAX_DRAWERS);
    for (int index = 0; index < drawerCount; index++) {
        this->drawers[index] = drawers[index];
    }
}

int DrawerManager::getDrawers(Drawer outputDrawers[], int maxDrawers) const {
    int count = min(drawerCount, maxDrawers);
    for (int index = 0; index < count; index++) {
        outputDrawers[index] = drawers[index];
    }

    return count;
}

int DrawerManager::getDrawerCount() const {
    return drawerCount;
}

Drawer* DrawerManager::getDrawer(int drawerId) {
    for (int index = 0; index < drawerCount; index++) {
        if (drawers[index].getId() == drawerId) {
            return &drawers[index];
        }
    }

    return nullptr;
}

const Drawer* DrawerManager::getDrawer(int drawerId) const {
    for (int index = 0; index < drawerCount; index++) {
        if (drawers[index].getId() == drawerId) {
            return &drawers[index];
        }
    }

    return nullptr;
}

bool DrawerManager::configureDrawer(int drawerId, const String& medicationName, bool enabled, int pillCount) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer == nullptr) {
        return false;
    }

    drawer->setMedicationName(medicationName);
    drawer->setEnabled(enabled);
    drawer->setPillCount(pillCount);
    return true;
}

bool DrawerManager::recordDoseTaken(int drawerId, bool& drawerBecameEmpty) {
    drawerBecameEmpty = false;
    Drawer* drawer = getDrawer(drawerId);
    if (drawer == nullptr) {
        return false;
    }

    drawerBecameEmpty = drawer->recordDoseTaken();
    return true;
}

void DrawerManager::highlightDrawer(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer != nullptr) {
        drawer->highlight();
        highlightedDrawerId = drawerId;
        highlightLightsOn = true;
        lastHighlightToggleMs = millis();
        reminderColorIndex = 0;
        showHighlightedDrawer();
    }
}

void DrawerManager::stopHighlight(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer != nullptr) {
        drawer->stopHighlight();
    }
    if (highlightedDrawerId == drawerId) {
        highlightedDrawerId = 0;
        highlightLightsOn = false;
        clearDrawerLights();
    }
}

void DrawerManager::turnOffAllDrawers() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].stopHighlight();
    }
    highlightedDrawerId = 0;
    highlightLightsOn = false;
    clearDrawerLights();
}

bool DrawerManager::isDrawerOpen(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    return drawer != nullptr && drawer->isOpen();
}

bool DrawerManager::readDrawerState(int drawerId) {
    return isDrawerOpen(drawerId);
}

bool DrawerManager::isHighlightActive() const {
    return highlightedDrawerId > 0;
}

bool DrawerManager::isHighlightPulseOn() const {
    return highlightedDrawerId > 0 && highlightLightsOn;
}

void DrawerManager::update() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].update();
    }
    if (highlightedDrawerId <= 0) {
        return;
    }

    unsigned long now = millis();
    if (now - lastHighlightToggleMs < 500UL) {
        return;
    }

    lastHighlightToggleMs = now;
    highlightLightsOn = !highlightLightsOn;
    if (highlightLightsOn) {
        showHighlightedDrawer();
    } else {
        clearDrawerLights();
    }
}

void DrawerManager::showHighlightedDrawer() {
    if (highlightedDrawerId <= 0 || highlightedDrawerId > NEOPIXEL_DRAWER_COUNT) {
        clearDrawerLights();
        return;
    }

    drawerLights.clear();
    int drawerIndex = NEOPIXEL_DRAWER_COUNT - highlightedDrawerId;
    int firstPixelIndex = drawerIndex * NEOPIXELS_PER_DRAWER;
    uint32_t color = nextReminderColor();
    for (int offset = 0; offset < NEOPIXELS_PER_DRAWER; offset++) {
        drawerLights.setPixelColor(firstPixelIndex + offset, color);
    }
    drawerLights.show();
}

void DrawerManager::clearDrawerLights() {
    drawerLights.clear();
    drawerLights.show();
}

uint32_t DrawerManager::nextReminderColor() {
    uint32_t colors[] = {
        drawerLights.Color(32, 16, 0),
        drawerLights.Color(0, 28, 24),
        drawerLights.Color(28, 0, 24),
        drawerLights.Color(0, 32, 0),
        drawerLights.Color(0, 0, 32),
    };
    uint32_t color = colors[reminderColorIndex % (sizeof(colors) / sizeof(colors[0]))];
    reminderColorIndex++;
    return color;
}
