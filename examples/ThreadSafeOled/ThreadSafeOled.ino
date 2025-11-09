#include <Arduino.h>

#include "DisplayMenu.h"

#define DELAY_TIME 1000

// GyverOLED<SSD1306_128x64> oled;    Do not use display management directly through this library!!!
ThreadSafeOLED safeOled;           // Use this wrapper over GyverOLED for thread-safe display output
DisplayMenu menu(&safeOled);

void setup()
{
  Serial.begin(115200);

  menu.setup();

  menu.addItem("Item1");
  menu.addItem("Item2");
  menu.addItem("Item3");

  while (!Serial) delay(10);
}

void treadSafePaint()
{
  // We receive a copy and immediately block the use of the display from multiple places at once, for correct output
  const auto oled = safeOled.acquire();

  oled->print("text");
  oled->update();

  // After the “oled” variable is out of scope, the display is unlocked for use
}

void treadSafePaint2()
{
  // We receive a copy and immediately block the use of the display from multiple places at once, for correct output
  auto oled = safeOled.acquire();
  oled->print("text1");

  // In this function, we again have a lock that will prevent the code from executing.
  // Therefore, we need to unlock the display
  oled.unlock();
  treadSafePaint();

  // And again locked
  oled.lock();
  oled->print("text3");

  // After the “oled” variable is out of scope, the display is unlocked for use
}

void loop()
{
  treadSafePaint();
  delay(DELAY_TIME);
  treadSafePaint2();
  delay(DELAY_TIME);
}