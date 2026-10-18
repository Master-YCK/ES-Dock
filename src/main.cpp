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
constexpr int8_t LCD_SCK = 40;
constexpr int8_t LCD_QSPI_D0 = 45;
constexpr int8_t LCD_QSPI_D1 = 48;
constexpr int8_t LCD_QSPI_D2 = 47;
constexpr int8_t LCD_QSPI_D3 = 21;
constexpr int8_t LCD_RST = 42;
constexpr int8_t TOUCH_SDA = 12;
constexpr int8_t TOUCH_SCL = 16;
constexpr uint8_t AXS15231B_I2C_ADDRESS = 0x3B;

// The backlight pin is not present in the supplied pin map.
constexpr int8_t LCD_BL = 14;

Arduino_DataBus *lcdBus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3);
Arduino_GFX *lcd = new Arduino_AXS15231B(
  lcdBus, LCD_RST, 0, false, 180, 640);

bool probeAxs15231b()
{
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  Wire.setClock(400000);
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
  probeAxs15231b();

  if (!lcd->begin(20000000UL))
  {
    Serial.println("[Fatal] Physical LCD init failed. Halting.");
    while (true)
      delay(1000);
  }

  // This panel requires inverted color output for RGB565 pixels.
  lcd->invertDisplay(true);

  if (LCD_BL >= 0)
  {
    pinMode(LCD_BL, OUTPUT);
    // The OSPTEK example defines the LCD backlight as active-high.
    digitalWrite(LCD_BL, HIGH);
  }
  delay(100);

  // Clear any residual pixels left by the panel initialization sequence.
  lcd->fillScreen(0x0000);
  lcd->setCursor(5, 5);
  lcd->setTextColor(0xFFFF, 0x0000);
  lcd->setTextSize(2);
  lcd->print("Hello World");

  Serial.println("[Display] AXS15231B physical LCD test completed");
  NetworkService::init();
  Serial.println("[System] System ready for App connection.");
}

void loop()
{
  NetworkService::loop();
  delay(2);
}