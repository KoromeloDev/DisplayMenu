#ifndef DISPLAYMENU_H
#define DISPLAYMENU_H

#include <GyverOLED.h>
#include <vector>

#include "MenuItem.h"

using namespace std;

class DisplayMenu
{
public:
  explicit DisplayMenu(GyverOLED<SSD1306_128x64> *oled);
  virtual ~DisplayMenu() = default;

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
    m_items.emplace_back(text, value);
    addItem();
  }
  template <typename PTR_T, typename T>
  void addItem(const String &text, PTR_T value, T min)
  {
    m_items.emplace_back(text, value, min);
    addItem();
  }
  template <typename PTR_T, typename T>
  void addItem(const String &text, PTR_T value, T min, T max)
  {
    m_items.emplace_back(text, value, min, max);
    addItem();
  }

  void deleteItem(uint8_t index);

  void repaint() const;

protected:
  virtual void paintSelectedItem() const;
  virtual void paintUnselectedItem(uint8_t index) const;
  virtual void addItem() const;
  virtual void valuePrint(const MenuItem &item, uint8_t start) const;

private:
  GyverOLED<SSD1306_128x64> *m_oled;
  vector<MenuItem> m_items;
  bool m_menuLoop;
  uint8_t m_selectedIndex;
  int8_t m_scrollItemCount;


  void scrollUp();
  void scrollDown();
};

#endif // DISPLAYMENU_H