#include <Arduino.h>

#include "DisplayMenu.h"

#define DELAY_TIME 2500

// GyverOLED<SSD1306_128x64> oled;    Do not use display management directly through this library!!!
ThreadSafeOLED safeOled;           // Use this wrapper over GyverOLED for thread-safe display output
DisplayMenu menu(&safeOled);

bool testBoolValue = false;
uint8_t testUintValue = 0;
float testFloatValue = 0.0f;
uint8_t testUintValueMin = 15;
uint8_t testUintValueMinMax = 12;

void setup()
{
  menu.setup();

  menu.addItem("BOOL", &testBoolValue);
  menu.addItem("UINT", &testUintValue);
  menu.addItem("FLOAT", &testFloatValue);
  menu.addItem("UINT MIN", &testUintValueMin, 10);
  menu.addItem("UINT MIN MAX", &testUintValueMinMax, 11, 14);
}

void setFloatValue(float value)
{
  testFloatValue = value;
  menu.repaint();
}

void loop()
{
  setFloatValue(15.6f);
  delay(DELAY_TIME);
  setFloatValue(5.3f);

  // Increment bool value
  menu.incrementValue();
  delay(500);
  menu.incrementValue();
  delay(DELAY_TIME);
  // Decrement bool value. Equivalent to the code above
  menu.decrementValue();
  delay(500);
  menu.decrementValue();
  delay(DELAY_TIME);

  // Increment uint8_t value
  menu.selectNext();
  menu.incrementValue();
  for (uint8_t i = 0; i < 10; ++i)
  {
    delay(500);
    menu.incrementValue();
  }
  delay(DELAY_TIME);
  // Decrement uint8_t value
  menu.decrementValue();
  for (uint8_t i = 0; i < 11; ++i)    // Uint8_t has a range of 0-255, if you subtract from 0 you get 255
  {
    delay(500);
    menu.decrementValue();
  }
  delay(DELAY_TIME);

  // Increment float value    // For "double", the code works similarly.
  menu.selectNext();
  menu.incrementValue();
  for (uint8_t i = 0; i < 10; ++i)
  {
    delay(500);
    menu.incrementValue();
  }
  delay(DELAY_TIME);
  // Decrement float value    // For "double", the code works similarly.
  menu.decrementValue();
  for (uint8_t i = 0; i < 20; ++i)
  {
    delay(500);
    menu.decrementValue();
  }
  delay(DELAY_TIME);

  // Decreases the value to a minimum
  menu.selectNext();
  for (uint8_t i = 0; i < 10; ++i)
  {
    delay(500);
    menu.decrementValue();
  }
  delay(DELAY_TIME);


  // Decreases the value to a minimum
  menu.selectNext();
  for (uint8_t i = 0; i < 4; ++i)
  {
    delay(500);
    menu.decrementValue();
  }
  delay(DELAY_TIME);
  // Increases the value to a maximum
  for (uint8_t i = 0; i < 8; ++i)
  {
    delay(500);
    menu.incrementValue();
  }
  delay(DELAY_TIME);
}