// src/status_led.hpp
#pragma once
#include <Arduino.h>

#ifndef RGB_LED_PIN
#define RGB_LED_PIN 48 // ESP32-S3-N16R8 / WROOM-2 onboard RGB LED
#endif

class StatusLed
{
public:
    static void init();
    static void setRed();    // 無設備連線
    static void setYellow(); // Wi-Fi 已連線，等待 App
    static void setGreen();  // App WebSocket 已連線
    static void setOff();
    static void setColor(uint8_t r, uint8_t g, uint8_t b);
};