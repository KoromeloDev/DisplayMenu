#pragma once

#include <GyverOLED.h>
#include <vector>

#include "MenuItem.h"

using namespace std;

class DisplayMenu
{
public:
  explicit DisplayMenu(GyverOLED<SSD1306_128x64> *oled);

  void setup() const;
  void setMenuLoop(bool enable);

  void selectNext();
  void selectPrevious();

  void incrementValue() const;
  void decrementValue() const;

  [[nodiscard]] MenuItem getItem(uint8_t index);
  [[nodiscard]] MenuItem getSelectedItem() const;
  [[nodiscard]] uint8_t getSelectedIndex() const;
  [[nodiscard]] String getSelectedText() const;

  void addItem(const String &text);
  template <typename PTR_T>
  void addItem(const String &text, PTR_T value)
  {
    _items.emplace_back(text, value);
    addItem();
  }
  template <typename PTR_T, typename T>
  void addItem(const String &text, PTR_T value, T min)
  {
    _items.emplace_back(text, value, min);
    addItem();
  }
  template <typename PTR_T, typename T>
  void addItem(const String &text, PTR_T value, T min, T max)
  {
    _items.emplace_back(text, value, min, max);
    addItem();
  }

  void deleteItem(uint8_t index);

  void repaint() const;

private:
  GyverOLED<SSD1306_128x64> *_oled;
  vector<MenuItem> _items;
  bool _menuLoop;
  uint8_t _selectedIndex;
  int8_t _scrollItemCount;

  void selectIndex() const;
  void unselectIndex(uint8_t index) const;
  void scrollUp();
  void scrollDown();
  void addItem() const;
  void valuePrint(const MenuItem &item, uint8_t start) const;
};
