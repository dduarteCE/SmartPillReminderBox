#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "config/SystemConfig.h"
#include "models/Drawer.h"

class DrawerManager {
public:
    DrawerManager();

    void begin();
    void reset();
    void setDrawers(const Drawer drawers[], int count);
    int getDrawers(Drawer outputDrawers[], int maxDrawers) const;
    int getDrawerCount() const;
    Drawer* getDrawer(int drawerId);
    const Drawer* getDrawer(int drawerId) const;
    bool configureDrawer(int drawerId, const String& medicationName, bool enabled, int pillCount);
    bool recordDoseTaken(int drawerId, bool& drawerBecameEmpty);
    void highlightDrawer(int drawerId);
    void stopHighlight(int drawerId);
    void turnOffAllDrawers();
    bool isDrawerOpen(int drawerId);
    bool readDrawerState(int drawerId);
    bool isHighlightActive() const;
    bool isHighlightPulseOn() const;
    void update();

private:
    void showHighlightedDrawer();
    void clearDrawerLights();
    uint32_t nextReminderColor();

    Drawer drawers[MAX_DRAWERS];
    int drawerCount;
    Adafruit_NeoPixel drawerLights;
    int highlightedDrawerId;
    bool highlightLightsOn;
    unsigned long lastHighlightToggleMs;
    size_t reminderColorIndex;
};
