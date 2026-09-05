// src/status_led.cpp
#include "hardware/status_led.hpp"

constexpr uint8_t BRIGHTNESS = 30; // 亮度縮放 (0~255)

void StatusLed::init()
{
    pinMode(RGB_LED_PIN, OUTPUT);
    setRed(); // 開機預設紅燈
}

void StatusLed::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    pinMode(RGB_LED_PIN, OUTPUT);

    const uint8_t scaled_r = static_cast<uint16_t>(r) * BRIGHTNESS / 255U;
    const uint8_t scaled_g = static_cast<uint16_t>(g) * BRIGHTNESS / 255U;
    const uint8_t scaled_b = static_cast<uint16_t>(b) * BRIGHTNESS / 255U;
    neopixelWrite(RGB_LED_PIN, scaled_r, scaled_g, scaled_b);
}

void StatusLed::setRed()
{
    setColor(255, 0, 0);
}

void StatusLed::setYellow()
{
    // 紅 + 綠 = 黃
    setColor(255, 200, 0);
}

void StatusLed::setGreen()
{
    setColor(0, 255, 0);
}

void StatusLed::setOff()
{
    setColor(0, 0, 0);
}