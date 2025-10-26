#include <Arduino.h>

#include "DisplayMenu.h"

#define DELAY_TIME 1000

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
  delay(DELAY_TIME);
  // Secure deletion of an item. The last 2 times the code won't delete anything,
  // because everything will already be deleted.
  for (uint8_t i = 0; i <= 12; ++i)
  {
    menu.deleteItem(0);
    delay(DELAY_TIME);
  }

  for (uint8_t i = 1; i <= 10; ++i)
  {
    menu.addItem(String(i));
  }
  menu.selectPrevious();

  // Deleting in reverse order
  for (uint8_t i = 10; i > 0; --i)
  {
    menu.deleteItem(i - 1);
    delay(DELAY_TIME);
  }
}