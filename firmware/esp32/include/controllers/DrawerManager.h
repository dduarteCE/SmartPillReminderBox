#pragma once

#include <Arduino.h>
#include "config/SystemConfig.h"
#include "models/Drawer.h"

class DrawerManager {
public:
    DrawerManager();

    void begin();
    void setDrawers(const Drawer drawers[], int count);
    int getDrawers(Drawer outputDrawers[], int maxDrawers) const;
    int getDrawerCount() const;
    Drawer* getDrawer(int drawerId);
    const Drawer* getDrawer(int drawerId) const;
    bool configureDrawer(int drawerId, const String& medicationName, bool enabled);
    void highlightDrawer(int drawerId);
    void stopHighlight(int drawerId);
    void turnOffAllDrawers();
    bool isDrawerOpen(int drawerId);
    bool readDrawerState(int drawerId);
    void update();

private:
    Drawer drawers[MAX_DRAWERS];
    int drawerCount;
};
