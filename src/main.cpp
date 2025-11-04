#include <Arduino.h>

#include "DisplayMenu.h"

GyverOLED<SSD1306_128x64> oled;
DisplayMenu menu(&oled);

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