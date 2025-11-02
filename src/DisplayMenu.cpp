#include "DisplayMenu.h"

#define SELECTED_HEIGHT 10
#define SELECTED_LEFT_OFFSET 8
#define VALUE_RIGHT_OFFSET 0
#define CHAR_WIDTH 6

DisplayMenu::DisplayMenu(GyverOLED<SSD1306_128x64> *oled) : m_oled(oled)
{
  m_selectedIndex = 0;
  m_scrollItemCount = 0;
  m_menuLoop = true;
}

void DisplayMenu::setup() const
{
  m_oled->init();
  Wire.setClock(800000L);
  m_oled->clear();
  m_oled->update();
}

void DisplayMenu::setMenuLoop(const bool enable)
{
  m_menuLoop = enable;
}

void DisplayMenu::selectNext()
{
  if (m_items.empty()) return;

  if ((m_selectedIndex + 1) * SELECTED_HEIGHT + SELECTED_HEIGHT > 63)
  {
    scrollDown();
    return;
  }

  const uint8_t index = m_items.size() - 1;

  if (m_menuLoop)
  {
      paintUnselectedItem(m_selectedIndex);
      m_selectedIndex = m_selectedIndex == index ? 0 : ++m_selectedIndex;
  }
  else
  {
    if (m_selectedIndex == index) return;
    paintUnselectedItem(m_selectedIndex);
    ++m_selectedIndex;
  }

  paintSelectedItem();
  m_oled->update();
}

void DisplayMenu::selectPrevious()
{
  if (m_items.empty()) return;

  if (m_selectedIndex == 0)
  {
    scrollUp();
    return;
  }

  const uint8_t index = m_items.size() - 1;

  if (m_menuLoop)
  {
    paintUnselectedItem(m_selectedIndex);
    m_selectedIndex = m_selectedIndex == 0 ? index : --m_selectedIndex;
  }
  else
  {
    if (m_selectedIndex == 0) return;
    paintUnselectedItem(m_selectedIndex);
    --m_selectedIndex;
  }

  paintSelectedItem();
  m_oled->update();
}

MenuItem DisplayMenu::getSelectedItem() const
{
  return m_items.at(getSelectedIndex());
}

void DisplayMenu::addItem(const String &text)
{
  m_items.emplace_back(text);
  addItem();
}

void DisplayMenu::incrementValue() const
{
  if (m_items.empty()) return;
  getSelectedItem().incrementValue();
  paintSelectedItem();
  m_oled->update();
}

void DisplayMenu::decrementValue() const
{
  if (m_items.empty()) return;
  getSelectedItem().decrementValue();
  paintSelectedItem();
  m_oled->update();
}

MenuItem DisplayMenu::getItem(const uint8_t index)
{
  return m_items.at(index);
}

uint8_t DisplayMenu::getSelectedIndex() const
{
  return m_selectedIndex + m_scrollItemCount;
}

String DisplayMenu::getSelectedText() const
{
  return m_items.at(getSelectedIndex()).getText();
}

void DisplayMenu::scrollUp()
{
  if (m_items.empty()) return;
  const auto nextIndex = static_cast<int8_t>(m_selectedIndex + m_scrollItemCount - 1);

  if (m_menuLoop)
  {
    if (nextIndex == -1)
    {
      m_selectedIndex = 5;                             // The last visible index
      m_scrollItemCount = static_cast<int8_t>(m_items.size() - 1 - m_selectedIndex);
    }
    else
    {
      --m_scrollItemCount;
    }
  }
  else
  {
    if (nextIndex == -1) return;
    --m_scrollItemCount;
  }

  repaint();
}

void DisplayMenu::scrollDown()
{
  if (m_items.empty()) return;
  const auto nextIndex = static_cast<int8_t>(m_selectedIndex + m_scrollItemCount + 1);

  if (m_menuLoop)
  {
    if (nextIndex > m_items.size() - 1)
    {
      m_scrollItemCount = 0;
      m_selectedIndex = 0;
    }
    else
    {
      ++m_scrollItemCount;
    }
  }
  else
  {
    if (nextIndex > m_items.size() - 1) return;
    ++m_scrollItemCount;
  }

  repaint();
}

void DisplayMenu::deleteItem(const uint8_t index)
{
  if (index >= m_items.size()) return;
  m_items.erase(m_items.begin() + index);

  if (index == getSelectedIndex() || getSelectedIndex() >= m_items.size())
  {
    if (m_scrollItemCount > 0)
    {
      scrollUp();
      return;
    }

    if (m_selectedIndex > 0)
    {
      --m_selectedIndex;
    }
  }

  m_oled->clear();
  repaint();
}

void DisplayMenu::repaint() const
{
  const uint8_t sizeToPrint = m_items.size() >= 6 ? 6 : m_items.size();

  for (uint8_t i = 0; i < sizeToPrint; ++i)
  {
    if (i == m_selectedIndex) paintSelectedItem();
    else paintUnselectedItem(i);
  }

  m_oled->update();
}

void DisplayMenu::addItem() const
{
  if (const uint8_t index = m_items.size() - 1; index == m_selectedIndex)
  {
    paintSelectedItem();
  }
  else
  {
    paintUnselectedItem(index);
  }

  m_oled->update();
}

void DisplayMenu::valuePrint(const MenuItem &item, const uint8_t start) const
{
  if (!item.hasValue()) return;
  const String valueString = item.getValueAsString();
  m_oled->setCursorXY(static_cast<uint8_t>(127 - VALUE_RIGHT_OFFSET - valueString.length() * CHAR_WIDTH), start);
  m_oled->print(valueString);
}

void DisplayMenu::paintSelectedItem() const
{
  if (m_items.empty()) return;

  const uint8_t start = m_selectedIndex * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 2 >= 63 ? 63 : start + SELECTED_HEIGHT - 2;
  m_oled->rect(0, start, 128, end, OLED_FILL);
  m_oled->setCursorXY(SELECTED_LEFT_OFFSET, start + 1);
  m_oled->textMode(BUF_SUBTRACT);
  m_oled->print(getSelectedText());
  valuePrint(getSelectedItem(), start + 1);
}

void DisplayMenu::paintUnselectedItem(const uint8_t index) const
{
  const uint8_t start = index * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 1 >= 63 ? 63 : start + SELECTED_HEIGHT - 1;
  const MenuItem item = m_items.at(index + m_scrollItemCount);
  m_oled->clear(0, start, 128, end);
  m_oled->setCursorXY(0, start);
  m_oled->textMode(BUF_ADD);
  m_oled->print(item.getText());
  valuePrint(item, start);
}
