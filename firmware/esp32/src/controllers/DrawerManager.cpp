#include "controllers/DrawerManager.h"

#include "config/PinConfig.h"

DrawerManager::DrawerManager()
    : drawers{
        Drawer(1, "", false, LED_DRAWER_1_PIN, REED_DRAWER_1_PIN),
        Drawer(2, "", false, LED_DRAWER_2_PIN, REED_DRAWER_2_PIN)
      },
      drawerCount(2) {}

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
