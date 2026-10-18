#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <lvgl.h>
#include "virtual_screen/virtual_display.hpp"
#include "virtual_screen/home.hpp"
#include "network/network.hpp"
#include "hardware/status_led.hpp"

constexpr uint32_t SERIAL_BAUD_RATE = 115200;

constexpr int8_t LCD_CS = 39;
constexpr int8_t LCD_SCK = 41;
constexpr int8_t LCD_QSPI_D0 = 45;
constexpr int8_t LCD_QSPI_D1 = 48;
constexpr int8_t LCD_QSPI_D2 = 47;
constexpr int8_t LCD_QSPI_D3 = 21;
constexpr int8_t LCD_RST = 42;
constexpr int8_t LCD_MODE = 17;
constexpr int8_t TOUCH_SDA = 12;
constexpr int8_t TOUCH_SCL = 16;
constexpr uint8_t AXS15231B_I2C_ADDRESS = 0x3B;

// The backlight pin is not present in the supplied pin map.
constexpr int8_t LCD_BL = 14;

Arduino_DataBus *lcdBus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3);
Arduino_GFX *lcd = new Arduino_AXS15231B(
  lcdBus, LCD_RST, 0, false, 174, 640);

bool probeAxs15231b()
{
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  Wire.setClock(400000);

  Serial.println("[I2C] Scanning addresses...");
  uint8_t deviceCount = 0;
  for (uint8_t address = 1; address < 127; ++address)
  {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
      Serial.printf("[I2C] Device found at 0x%02X\n", address);
      ++deviceCount;
    }
  }

  if (deviceCount == 0)
  {
    Serial.println("[I2C] No devices found");
  }

  Wire.beginTransmission(AXS15231B_I2C_ADDRESS);
  const uint8_t error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.println("[Display] AXS15231B I2C controller responded at 0x3B");
    return true;
  }

  Serial.printf("[Display] AXS15231B I2C probe failed, error=%u\n", error);
  return false;
}

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);

  Serial.println("\n=== ESP32-S3 AXS15231B LCD Test ===");

  // LCM_IM high selects the QSPI interface on the adapter board.
  pinMode(LCD_MODE, OUTPUT);
  digitalWrite(LCD_MODE, HIGH);

  if (!lcd->begin(10000000UL))
  {
    Serial.println("[Fatal] Physical LCD init failed. Halting.");
    while (true)
      delay(1000);
  }

  // RST is shared by the LCD and touch controller; probe touch after the
  // display reset sequence has completed.
  probeAxs15231b();
  VirtualDisplay::setTouchBus(&Wire, AXS15231B_I2C_ADDRESS);

  if (LCD_BL >= 0)
  {
    pinMode(LCD_BL, OUTPUT);
    // The OSPTEK example defines the LCD backlight as active-high.
    digitalWrite(LCD_BL, HIGH);
  }
  delay(100);

  VirtualDisplay::setHardwareDisplay(lcd);
  if (!VirtualDisplay::init())
  {
    Serial.println("[Fatal] LVGL display initialization failed. Halting.");
    while (true)
      delay(1000);
  }
  lv_tick_set_cb([]() -> uint32_t { return millis(); });
  createDemoUI();
  lv_timer_handler();

  Serial.println("[Display] LVGL physical LCD test completed");
  NetworkService::init();
  Serial.println("[System] System ready for App connection.");
}

void loop()
{
  NetworkService::loop();
  lv_timer_handler();
  delay(5);
}