#include "controllers/DrawerManager.h"

#include "config/PinConfig.h"

DrawerManager::DrawerManager()
    : drawers{
        Drawer(1, "", false, LED_DRAWER_1_PIN, REED_DRAWER_1_PIN),
        Drawer(2, "", false, LED_DRAWER_2_PIN, REED_DRAWER_2_PIN),
        Drawer(3, "", false, LED_DRAWER_3_PIN, REED_DRAWER_3_PIN),
        Drawer(4, "", false, LED_DRAWER_4_PIN, REED_DRAWER_4_PIN),
        Drawer(5, "", false, LED_DRAWER_5_PIN, REED_DRAWER_5_PIN),
        Drawer(6, "", false, LED_DRAWER_6_PIN, REED_DRAWER_6_PIN),
        Drawer(7, "", false, LED_DRAWER_7_PIN, REED_DRAWER_7_PIN)
      },
      drawerCount(MAX_DRAWERS) {}

void DrawerManager::begin() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].begin();
    }
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

bool DrawerManager::configureDrawer(int drawerId, const String& medicationName, bool enabled) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer == nullptr) {
        return false;
    }

    drawer->setMedicationName(medicationName);
    drawer->setEnabled(enabled);
    return true;
}

void DrawerManager::highlightDrawer(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer != nullptr) {
        drawer->highlight();
    }
}

void DrawerManager::stopHighlight(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    if (drawer != nullptr) {
        drawer->stopHighlight();
    }
}

void DrawerManager::turnOffAllDrawers() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].stopHighlight();
    }
}

bool DrawerManager::isDrawerOpen(int drawerId) {
    Drawer* drawer = getDrawer(drawerId);
    return drawer != nullptr && drawer->isOpen();
}

bool DrawerManager::readDrawerState(int drawerId) {
    return isDrawerOpen(drawerId);
}

void DrawerManager::update() {
    for (int index = 0; index < drawerCount; index++) {
        drawers[index].update();
    }
}
