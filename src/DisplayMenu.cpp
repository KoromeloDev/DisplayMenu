#include "DisplayMenu.h"

#define SELECTED_HEIGHT 10
#define SELECTED_LEFT_OFFSET 8

#define VALUE_RIGHT_OFFSET 0
#define VALUE_LEFT_OFFSET 10

#define LEFT_OFFSET 1
#define CHAR_WIDTH 6

#define DISPLAY_WIDTH 127
#define DISPLAY_HEIGHT 63

DisplayMenu::DisplayMenu(GyverOLED<SSD1306_128x64> *oled) : m_safeOled(oled)
{
  m_selectedIndex = 0;
  m_scrollItemCount = 0;
  m_menuLoop = true;
  m_scale = 1;
}

void DisplayMenu::setup() const
{
  auto oled = m_safeOled.acquire();
  oled->init();
  Wire.setClock(800000L);
  oled->clear();
  oled->update();
}

void DisplayMenu::setMenuLoop(const bool enable)
{
  m_menuLoop = enable;
}

void DisplayMenu::setScale(const uint8_t scale)
{
  if (scale >= 1 && scale <= 4)
  {
    m_scale = scale;
  }
  else
  {
    m_scale = 1;
  }

  m_safeOled.acquire()->setScale(m_scale);
}

void DisplayMenu::selectNext()
{
  if (m_items.empty()) return;

  if ((m_selectedIndex + 1) * SELECTED_HEIGHT + SELECTED_HEIGHT > DISPLAY_HEIGHT)
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
  m_safeOled.acquire()->update();
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
  m_safeOled.acquire()->update();
}

void DisplayMenu::incrementValue()
{
  if (m_items.empty()) return;
  getSelectedItem().incrementValue();
  paintSelectedItem();
  m_safeOled.acquire()->update();
}

void DisplayMenu::decrementValue()
{
  if (m_items.empty()) return;
  getSelectedItem().decrementValue();
  paintSelectedItem();
  m_safeOled.acquire()->update();
}

MenuItem DisplayMenu::getItem(const uint8_t index)
{
  return m_items.at(index);
}

MenuItem DisplayMenu::getSelectedItem() const
{
  return m_items.at(getSelectedIndex());
}

uint8_t DisplayMenu::getSelectedIndex() const
{
  return m_selectedIndex + m_scrollItemCount;
}

String DisplayMenu::getSelectedText() const
{
  return m_items.at(getSelectedIndex()).getText();
}

void DisplayMenu::addItem(const String &text)
{
  m_items.emplace_back(text);
  addItem();
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

  repaint();
}

void DisplayMenu::repaint()
{
  const uint8_t sizeToPrint = m_items.size() >= 6 ? 6 : m_items.size();
  auto oled = m_safeOled.acquire();
  oled->clear();

  for (uint8_t i = 0; i < sizeToPrint; ++i)
  {
    if (i == m_selectedIndex) paintSelectedItem();
    else paintUnselectedItem(i);
  }

  oled->update();
}

void DisplayMenu::paintSelectedItem(String text)
{
  if (m_items.empty()) return;

  const uint8_t start = m_selectedIndex * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 2 >= DISPLAY_HEIGHT ? DISPLAY_HEIGHT : start + SELECTED_HEIGHT - 2;
  auto oled = m_safeOled.acquire();
  oled->rect(0, start, DISPLAY_WIDTH, end, OLED_FILL);
  oled->setCursorXY(SELECTED_LEFT_OFFSET, start + 1);
  oled->textMode(BUF_SUBTRACT);

  if (text.isEmpty())
  {
    text = getAvaliableString(getSelectedItem(), true);
    oled->print(text);
    oled.unlock();
    valuePrint(getSelectedItem(), start + 1);
    oled.lock();
    oled->update();
    thread t(&DisplayMenu::scrollRight, this, getSelectedIndex(), 0);
    t.detach();
  }
  else
  {
    oled->print(text);
    oled.unlock();
    valuePrint(getSelectedItem(), start + 1);
    oled.lock();
    oled->update();
  }
}

void DisplayMenu::paintUnselectedItem(const uint8_t index)
{
  const uint8_t start = index * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 1 >= DISPLAY_HEIGHT ? DISPLAY_HEIGHT : start + SELECTED_HEIGHT - 1;
  const MenuItem item = m_items.at(index + m_scrollItemCount);
  auto oled = m_safeOled.acquire();
  oled->clear(0, start, DISPLAY_WIDTH, end);
  oled->setCursorXY(LEFT_OFFSET, start);
  oled->textMode(BUF_ADD);
  const String text = getAvaliableString(item, false);
  oled->print(text);
  oled.unlock();
  valuePrint(item, start);
}

void DisplayMenu::addItem()
{
  if (const uint8_t index = m_items.size() - 1; index == m_selectedIndex)
  {
    paintSelectedItem();
  }
  else
  {
    paintUnselectedItem(index);
  }

  m_safeOled.acquire()->update();
}

void DisplayMenu::valuePrint(const MenuItem &item, const uint8_t start) const
{
  if (!item.hasValue()) return;
  const String valueString = item.getValueAsString();
  auto oled = m_safeOled.acquire();
  oled->setCursorXY(static_cast<uint8_t>(DISPLAY_WIDTH - VALUE_RIGHT_OFFSET - valueString.length() * CHAR_WIDTH),
                      start);
  oled->print(valueString);
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

String DisplayMenu::getAvaliableString(const MenuItem &item, const bool selected, const uint16_t charIndex) const
{
  const String text = item.getText();
  String result;
  uint8_t avaliableWidth = item.hasValue() ? DISPLAY_WIDTH - VALUE_RIGHT_OFFSET - VALUE_LEFT_OFFSET -
                           item.getValueAsString().length() * CHAR_WIDTH : DISPLAY_WIDTH;
  if (selected) avaliableWidth -= SELECTED_LEFT_OFFSET;

  if (text.length() * CHAR_WIDTH * m_scale > avaliableWidth)
  {
    const uint8_t avaliableChar = avaliableWidth / (CHAR_WIDTH * m_scale) + charIndex;
    result = text.substring(charIndex, avaliableChar);

    if (result.length() + charIndex >= text.length())
    {
      result += text.substring(0, avaliableChar - result.length() - charIndex);
    }
  }
  else
  {
    result = text;
  }

  return result;
}

void DisplayMenu::scrollRight(const uint8_t index, uint16_t charIndex)
{
  if (index != getSelectedIndex()) return;
  const MenuItem item = getItem(index);
  if (charIndex + 1 == item.getText().length()) charIndex = 0;
  const String result = getAvaliableString(item, true, charIndex);
  if (result == item.getText()) return;
  paintSelectedItem(result);
  this_thread::sleep_for(chrono::milliseconds(charIndex == 0 ? 1500 : 500));
  thread t(&DisplayMenu::scrollRight, this, index, charIndex + 1);
  t.detach();
}
