#include <Arduino.h>
#include <lvgl.h>
#include "virtual_screen/virtual_display.hpp"
#include "virtual_screen/home.hpp"
#include "network/network.hpp"
#include "hardware/status_led.hpp"

constexpr uint32_t SERIAL_BAUD_RATE = 115200;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);

  Serial.println("\n=== ESP32-S3 Virtual LVGL Node ===");
  if (!VirtualDisplay::init())
  {
    Serial.println("[Fatal] VirtualDisplay init failed. Halting.");
    while (true)
      delay(1000);
  }

  createDemoUI();
  NetworkService::init();
  Serial.println("[System] System ready for App connection.");
}

void loop()
{
  // Drive LVGL timers and event processing at a fixed five-millisecond interval.
  static uint32_t lastTick = 0;
  const uint32_t now = millis();
  if (now - lastTick >= 5U)
  {
    lv_timer_handler();
    lastTick = now;
  }

  NetworkService::loop();
  delay(2);
}