#include <Arduino.h>

#include "DisplayMenu.h"

#define DELAY_TIME 500

GyverOLED<SSD1306_128x64> oled;
DisplayMenu menu(&oled);

void setup()
{
  menu.setup();

  for (uint8_t i = 1; i <= 10; ++i)
  {
    menu.addItem(String(i));
  }
}

void loop()
{
  // List down
  for (uint8_t i = 0; i <= 10; ++i)
  {
    delay(DELAY_TIME);
    menu.selectNext();
  }
  // List up
  for (uint8_t i = 0; i <= 10; ++i)
  {
    delay(DELAY_TIME);
    menu.selectPrevious();
  }

  delay(DELAY_TIME);
  menu.setMenuLoop(false);
  // List down
  for (uint8_t i = 0; i <= 10; ++i)
  {
    delay(DELAY_TIME);
    menu.selectNext();
  }
  // List up
  for (uint8_t i = 0; i <= 10; ++i)
  {
    delay(DELAY_TIME);
    menu.selectPrevious();
  }
}