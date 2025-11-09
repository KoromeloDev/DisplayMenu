#ifndef DISPLAYMENU_H
#define DISPLAYMENU_H

#include <GyverOLED.h>
#include <vector>

#include "MenuItem.h"
#include "ThreadSafeOLED.h"

using namespace std;

class DisplayMenu
{
public:
  explicit DisplayMenu(ThreadSafeOLED *oled);
  virtual ~DisplayMenu() = default;

  void setup() const;
  constexpr void setMenuLoop(bool enable);
  void setScale(uint8_t scale);

  void selectNext();
  void selectPrevious();

  void incrementValue();
  void decrementValue();

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

  virtual void repaint();

protected:
  virtual void paintSelectedItem(String text);
  virtual void paintUnselectedItem(uint8_t index);
  virtual void addItem();
  virtual void valuePrint(const MenuItem &item, uint8_t start) const;
  virtual void scrollUp();
  virtual void scrollDown();

  [[nodiscard]] ThreadSafeOLED *getSafeOled() const;
  [[nodiscard]] vector<MenuItem> getItems() const;
  [[nodiscard]] bool getMenuLoop() const;
  [[nodiscard]] uint8_t getScale() const;

private:
  ThreadSafeOLED *m_safeOled;
  vector<MenuItem> m_items;
  bool m_menuLoop;
  uint8_t m_selectedIndex;
  int8_t m_scrollItemCount;
  uint8_t m_scale;

  [[nodiscard]] String getAvaliableString(const MenuItem &item, bool selected, uint16_t charIndex = 0) const;
  void scrollRight(uint8_t index, uint16_t charIndex);
};

#endif // DISPLAYMENU_H
