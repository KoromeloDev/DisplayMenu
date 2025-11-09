#include <Arduino.h>

#include "DisplayMenu.h"

// GyverOLED<SSD1306_128x64> oled;    Do not use display management directly through this library!!!
ThreadSafeOLED safeOled;           // Use this wrapper over GyverOLED for thread-safe display output
DisplayMenu menu(&safeOled);

bool testBoolValue = false;

void setup()
{
  Serial.begin(115200);

  menu.setup();

  menu.addItem("Normal");
  menu.addItem("Horizontal out------------>");
  menu.addItem("Horizontal out + value------------>", &testBoolValue);

  menu.selectNext();
  menu.selectNext();

  while (!Serial) delay(10);
}

void loop()
{
  // delay(3000);
  // menu.selectNext();
}