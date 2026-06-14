#include "communication/WebSocketService.h"

WebSocketService::WebSocketService() : connectedClientsCount(0), lastEventJson("") {}

void WebSocketService::begin() {}

void WebSocketService::handleWebSocket() {}

void WebSocketService::loop() {
    handleWebSocket();
}

void WebSocketService::sendEvent(const DoseEvent& event) {
    lastEventJson = event.toJson();
}

void WebSocketService::sendReminderStarted(const DoseEvent& event) {
    sendEvent(event);
}

void WebSocketService::sendDrawerOpened(const DoseEvent& event) {
    sendEvent(event);
}

void WebSocketService::sendDrawerClosed(const DoseEvent& event) {
    sendEvent(event);
}

void WebSocketService::sendDoseCompleted(const DoseEvent& event) {
    sendEvent(event);
}

void WebSocketService::sendDoseMissed(const DoseEvent& event) {
    sendEvent(event);
}

void WebSocketService::broadcastEvent(const DoseEvent& event) {
    sendEvent(event);
}

bool WebSocketService::hasConnectedClient() const {
    return connectedClientsCount > 0;
}

int WebSocketService::connectedClientCount() const {
    return connectedClientsCount;
}
