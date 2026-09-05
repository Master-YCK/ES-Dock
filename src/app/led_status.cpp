// src/status_led.cpp
#include "hardware/status_led.hpp"
#include <FastLED.h>

constexpr uint8_t BRIGHTNESS = 30; // 亮度縮放 (0~255)
static CRGB s_led;

void StatusLed::init()
{
    FastLED.addLeds<WS2812, RGB_LED_PIN, GRB>(&s_led, 1);
    FastLED.setBrightness(BRIGHTNESS);
    setRed(); // 開機預設紅燈
}

void StatusLed::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    s_led = CRGB(r, g, b);
    FastLED.show();
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