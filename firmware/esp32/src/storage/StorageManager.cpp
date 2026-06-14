#include "storage/StorageManager.h"

StorageManager::StorageManager(const String& configFilePath) : configFilePath(configFilePath) {}

bool StorageManager::begin() {
    return true;
}

int StorageManager::loadDrawers(Drawer drawers[], int maxDrawers) {
    (void)drawers;
    (void)maxDrawers;
    return 0;
}

bool StorageManager::saveDrawers(const Drawer drawers[], int count) {
    (void)drawers;
    (void)count;
    return true;
}

int StorageManager::loadSchedules(Schedule schedules[], int maxSchedules) {
    (void)schedules;
    (void)maxSchedules;
    return 0;
}

bool StorageManager::saveSchedules(const Schedule schedules[], int count) {
    (void)schedules;
    (void)count;
    return true;
}

int StorageManager::loadEvents(DoseEvent events[], int maxEvents) {
    (void)events;
    (void)maxEvents;
    return 0;
}

bool StorageManager::saveEvents(const DoseEvent events[], int count) {
    (void)events;
    (void)count;
    return true;
}

bool StorageManager::clearAcknowledgedEvents(const int eventIds[], int count) {
    (void)eventIds;
    (void)count;
    return true;
}

String StorageManager::getConfigFilePath() const {
    return configFilePath;
}
