// src/network/network.cpp
#include "network/network.hpp"
#include "virtual_screen/virtual_display.hpp"
#include "hardware/status_led.hpp"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

#pragma pack(push, 1)
struct RemoteFrameHeader
{
    uint16_t magic = 0x4C56;
    uint8_t protocolVersion = 1;
    uint8_t messageType = 1;
    uint16_t width = LCD_WIDTH;
    uint16_t height = LCD_HEIGHT;
    uint16_t x = 0;
    uint16_t y = 0;
    uint32_t frameId = 0;
    uint32_t payloadLength = FRAME_BUFFER_SIZE;
    uint32_t reserved = 0;
};
#pragma pack(pop)

static uint32_t s_frameCounter = 0;

// Centralized state machine for the LED status.
static void updateLedStatus()
{
    const uint8_t stationCount = WiFi.softAPgetStationNum();
    const size_t webSocketCount = ws.count();
    const bool webSocketConnected = webSocketCount > 0;
    const bool wifiConnected = stationCount > 0;

    if (webSocketConnected)
    {
        StatusLed::setGreen();
    }
    else if (wifiConnected)
    {
        StatusLed::setYellow();
    }
    else
    {
        StatusLed::setRed();
    }
}

static void sendFullSnapshot(AsyncWebSocketClient *client)
{
    uint8_t *fb = VirtualDisplay::getFramebuffer();
    if (!fb || !client || client->status() != WS_CONNECTED)
        return;

    RemoteFrameHeader header;
    header.frameId = ++s_frameCounter;

    client->binary((uint8_t *)&header, sizeof(header));
    client->binary(fb, FRAME_BUFFER_SIZE);
}

static void onWsEvent(AsyncWebSocket *srv, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("[WS] Client #%u connected\n", client->id());

        JsonDocument doc;
        doc["type"] = "hello";
        doc["version"] = 1;
        doc["width"] = LCD_WIDTH;
        doc["height"] = LCD_HEIGHT;
        String json;
        serializeJson(doc, json);
        client->text(json);
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.printf("[WS] Client #%u disconnected\n", client->id());
        VirtualDisplay::updateRemotePointer(0, 0, false);
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->opcode == WS_TEXT)
        {
            data[len] = 0;
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, (char *)data);
            if (!err)
            {
                const char *msgType = doc["type"] | "";
                if (strcmp(msgType, "request_snapshot") == 0)
                {
                    sendFullSnapshot(client);
                }
                else if (strcmp(msgType, "pointer") == 0)
                {
                    int16_t x = doc["x"] | 0;
                    int16_t y = doc["y"] | 0;
                    const char *phase = doc["phase"] | "up";
                    bool pressed = (strcmp(phase, "down") == 0 || strcmp(phase, "move") == 0);
                    VirtualDisplay::updateRemotePointer(x, y, pressed);
                }
            }
        }
    }
}

// Listen for low-level Wi-Fi station events.
static void onWiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.printf("[WiFi] Device joined SoftAP. Total stations: %d\n", WiFi.softAPgetStationNum());
        break;

    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.printf("[WiFi] Device left SoftAP. Total stations: %d\n", WiFi.softAPgetStationNum());
        break;

    default:
        break;
    }
}

void NetworkService::init()
{
    constexpr uint8_t AP_CHANNEL = 1;
    constexpr uint8_t AP_MAX_CONNECTIONS = 4;
    constexpr const char *AP_PASSWORD = "Es-Dock@test123";
    constexpr const char *AP_SSID = "ES-Dock";

    // Register the Wi-Fi event callback.
    WiFi.onEvent(onWiFiEvent);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
    Serial.printf("[WiFi] SoftAP started. IP: %s\n", WiFi.softAPIP().toString().c_str());

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
    StatusLed::init(); // Initialize the onboard LED after the network peripherals.
    Serial.println("[HTTP] Server ready");
}

void NetworkService::loop()
{
    ws.cleanupClients();

    // Poll the actual connection state because AP events can run before the
    // station count has been updated.
    static uint32_t lastLedUpdate = 0;
    const uint32_t now = millis();
    if (now - lastLedUpdate >= 250U)
    {
        updateLedStatus();
        lastLedUpdate = now;
    }
}