// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   delay(1200);
//   int result = myFunction(2, 3);
//   Serial.println("ESP32-S3 started");
//   Serial.print("Result of myFunction(2, 3): " + String(result));
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   Serial.println("alive");
//   delay(1000);
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }

#include <Arduino.h>

constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 1000;

unsigned long lastHeartbeatMs = 0;

int myFunction(int x, int y);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);

  Serial.println("ES-Dock serial ready.");
  Serial.print("Baud rate: ");
  Serial.println(SERIAL_BAUD_RATE);
  int result = myFunction(2, 3);
  Serial.print("Result of myFunction(2, 3): ");
  Serial.println(result);
}

void loop()
{
  const unsigned long now = millis();

  if (now - lastHeartbeatMs >= HEARTBEAT_INTERVAL_MS)
  {
    lastHeartbeatMs = now;
    Serial.print("Heartbeat ms=");
    Serial.println(now);
  }
}

int myFunction(int x, int y)
{
  return x + y;
}