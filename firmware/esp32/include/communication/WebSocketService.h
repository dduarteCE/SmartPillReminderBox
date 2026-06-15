#pragma once

#include <Arduino.h>
#include <WebSocketsServer.h>
#include "models/DoseEvent.h"

class WebSocketService {
public:
    WebSocketService();

    void begin();
    void handleWebSocket();
    void loop();
    bool isEnabled() const;
    void sendEvent(const DoseEvent& event);
    bool hasConnectedClient() const;
    int connectedClientCount() const;

private:
    static void handleWebSocketEvent(
        uint8_t clientNum,
        WStype_t type,
        uint8_t* payload,
        size_t length
    );

    static WebSocketService* activeInstance;
    WebSocketsServer webSocketServer;
    int connectedClientsCount;
    String lastEventJson;
    bool enabled;
};
