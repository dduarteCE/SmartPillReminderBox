#include "communication/WebSocketService.h"

#include "config/SystemConfig.h"

WebSocketService* WebSocketService::activeInstance = nullptr;

WebSocketService::WebSocketService()
    : webSocketServer(WEBSOCKET_PORT),
      connectedClientsCount(0),
      lastEventJson(""),
      enabled(false) {}

void WebSocketService::begin() {
    activeInstance = this;
    webSocketServer.begin();
    webSocketServer.onEvent(WebSocketService::handleWebSocketEvent);
    enabled = true;
}

void WebSocketService::handleWebSocket() {
    if (!enabled) {
        return;
    }

    webSocketServer.loop();
}

void WebSocketService::loop() {
    handleWebSocket();
}

bool WebSocketService::isEnabled() const {
    return enabled;
}

void WebSocketService::sendEvent(const DoseEvent& event) {
    lastEventJson = event.toJson();
    if (!enabled) {
        return;
    }

    webSocketServer.broadcastTXT(lastEventJson);
}

bool WebSocketService::hasConnectedClient() const {
    return connectedClientsCount > 0;
}

int WebSocketService::connectedClientCount() const {
    return connectedClientsCount;
}

void WebSocketService::handleWebSocketEvent(
    uint8_t clientNum,
    WStype_t type,
    uint8_t* payload,
    size_t length
) {
    (void)clientNum;
    (void)payload;
    (void)length;

    if (activeInstance == nullptr) {
        return;
    }

    switch (type) {
        case WStype_CONNECTED:
            activeInstance->connectedClientsCount++;
            break;
        case WStype_DISCONNECTED:
            if (activeInstance->connectedClientsCount > 0) {
                activeInstance->connectedClientsCount--;
            }
            break;
        default:
            break;
    }
}
