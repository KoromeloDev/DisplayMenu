#include <Arduino.h>

#include "DisplayMenu.h"

GyverOLED<SSD1306_128x64> oled;
DisplayMenu menu(&oled);

bool testBoolValue = false;
uint8_t testUintValue = 0;
float testFloatValue = 0.0f;
uint8_t testUintValueMin = 15;
uint8_t testUintValueMinMax = 12;

void setup()
{
  Serial.begin(115200);

  menu.setup();

  menu.addItem("BOOL", &testBoolValue);
  menu.addItem("UINT", &testUintValue);
  menu.addItem("FLOAT", &testFloatValue);
  menu.addItem("UINT MIN", &testUintValueMin, 10);
  menu.addItem("UINT MIN MAX", &testUintValueMinMax, 11, 14);
  menu.addItem("Horizontal out------------>");
  menu.addItem("Vertical out");

  // menu.setMenuLoop(false);

  menu.selectNext();
  menu.selectNext();
  menu.selectNext();
  menu.selectNext();
  menu.selectNext();
  menu.selectNext();
  // menu.deleteItem(6);
  // menu.deleteItem(5);

  while (!Serial) delay(10);
}

void loop()
{
  delay(1000);
  menu.deleteItem(0);
  // testFloatValue = 15.6f;
  // menu.repaint();
  menu.selectNext();
  menu.incrementValue();
  // menu.decrementValue();
  // menu.selectPrevious();
}