#pragma once

#include <Arduino.h>
#include "models/DoseEvent.h"

class WebSocketService {
public:
    WebSocketService();

    void begin();
    void handleWebSocket();
    void loop();
    void sendEvent(const DoseEvent& event);
    void sendReminderStarted(const DoseEvent& event);
    void sendDrawerOpened(const DoseEvent& event);
    void sendDrawerClosed(const DoseEvent& event);
    void sendDoseCompleted(const DoseEvent& event);
    void sendDoseMissed(const DoseEvent& event);
    void broadcastEvent(const DoseEvent& event);
    bool hasConnectedClient() const;
    int connectedClientCount() const;

private:
    int connectedClientsCount;
    String lastEventJson;
};
