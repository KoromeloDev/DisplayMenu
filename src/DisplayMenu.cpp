#include <thread>

#include "DisplayMenu.h"

#define SELECTED_HEIGHT 10
#define SELECTED_LEFT_OFFSET 8

#define VALUE_RIGHT_OFFSET 0
#define VALUE_LEFT_OFFSET 10

#define LEFT_OFFSET 1
#define CHAR_WIDTH 6

#define DISPLAY_WIDTH 127
#define DISPLAY_HEIGHT 63

DisplayMenu::DisplayMenu(ThreadSafeOLED *oled) : m_safeOled(oled)
{
  m_selectedIndex = 0;
  m_scrollItemCount = 0;
  m_menuLoop = true;
  m_scale = 1;
}

void DisplayMenu::setup() const
{
  const auto oled = m_safeOled->get();
  oled->init();
  Wire.setClock(800000L);
  oled->clear();
  oled->update();
}

constexpr void DisplayMenu::setMenuLoop(const bool enable)
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
    return;
  }

  auto oled = m_safeOled->get();
  oled->setScale(m_scale);
  oled.unlock();
  repaint();
}

void DisplayMenu::selectNext()
{
  if (m_items.empty()) return;

  if (((m_selectedIndex + 1) * SELECTED_HEIGHT + SELECTED_HEIGHT) * m_scale > DISPLAY_HEIGHT)
  {
    scrollDown();
    return;
  }

  const uint8_t index = getSize() - 1;

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

  paintSelectedItem("");
  m_safeOled->get()->update();
}

void DisplayMenu::selectPrevious()
{
  if (m_items.empty()) return;

  if (m_selectedIndex == 0)
  {
    scrollUp();
    return;
  }

  const uint8_t index = getSize() - 1;

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

  paintSelectedItem("");
  m_safeOled->get()->update();
}

void DisplayMenu::incrementValue()
{
  if (m_items.empty()) return;
  getSelectedItem().incrementValue();
  paintSelectedItem("");
  m_safeOled->get()->update();
}

void DisplayMenu::decrementValue()
{
  if (m_items.empty()) return;
  getSelectedItem().decrementValue();
  paintSelectedItem("");
  m_safeOled->get()->update();
}

uint8_t DisplayMenu::getSize() const
{
  return m_items.size();
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
  if (index >= getSize()) return;
  m_items.erase(m_items.begin() + index);

  if (index == getSelectedIndex() || getSelectedIndex() >= getSize())
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

void DisplayMenu::clearItems()
{
  m_items.clear();
  m_selectedIndex = 0;
  m_scrollItemCount = 0;
  const auto oled = m_safeOled->get();
  oled->clear();
  oled->update();
}

void DisplayMenu::repaint()
{
  const uint8_t maxItems = DISPLAY_HEIGHT / (SELECTED_HEIGHT *  m_scale);
  const uint8_t sizeToPrint = getSize() >= maxItems ? maxItems : getSize();
  auto oled = m_safeOled->get();
  oled->clear();
  oled.unlock();

  for (uint8_t i = 0; i < sizeToPrint; ++i)
  {
    if (i == m_selectedIndex) paintSelectedItem("");
    else paintUnselectedItem(i);
  }

  oled.lock();
  oled->update();
}

void DisplayMenu::paintSelectedItem(String text)
{
  if (m_items.empty()) return;

  const bool textIsEmpty = text.isEmpty();
  const uint8_t start = m_selectedIndex * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 2 >= DISPLAY_HEIGHT ? DISPLAY_HEIGHT : start + SELECTED_HEIGHT - 2;
  auto oled = m_safeOled->get();
  oled->rect(0, start * m_scale, DISPLAY_WIDTH, end * m_scale, OLED_FILL);
  oled->setCursorXY(SELECTED_LEFT_OFFSET, (start + 1) * m_scale);
  oled->textMode(BUF_SUBTRACT);
  oled->setScale(m_scale);

  if (textIsEmpty)
  {
    text = getAvaliableString(getSelectedItem(), true);
  }

  if (!text.isEmpty())
  {
    oled->print(text);
  }

  oled.unlock();
  valuePrint(getSelectedItem(), start + 1);
  oled.lock();
  oled->update();

  if (textIsEmpty)
  {
    thread t(&DisplayMenu::scrollRight, this, getSelectedIndex(), 0);
    t.detach();
  }
}

void DisplayMenu::paintUnselectedItem(const uint8_t index)
{
  const uint8_t start = index * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 1 >= DISPLAY_HEIGHT ? DISPLAY_HEIGHT : start + SELECTED_HEIGHT - 1;
  const MenuItem item = m_items.at(index + m_scrollItemCount);
  auto oled = m_safeOled->get();
  oled->clear(0, start * m_scale, DISPLAY_WIDTH, end * m_scale);
  oled->setCursorXY(LEFT_OFFSET, start * m_scale);
  oled->textMode(BUF_ADD);
  const String text = getAvaliableString(item, false);
  oled->setScale(m_scale);
  oled->print(text);
  oled.unlock();
  valuePrint(item, start);
}

void DisplayMenu::addItem()
{
  if (const uint8_t index = getSize() - 1; index == m_selectedIndex)
  {
    paintSelectedItem("");
  }
  else
  {
    paintUnselectedItem(index);
  }

  m_safeOled->get()->update();
}

void DisplayMenu::valuePrint(const MenuItem &item, const uint8_t start) const
{
  if (!item.hasValue()) return;
  const String valueString = item.getValueAsString();
  const auto oled = m_safeOled->get();
  oled->setCursorXY(static_cast<uint8_t>(DISPLAY_WIDTH - VALUE_RIGHT_OFFSET - valueString.length() * CHAR_WIDTH *
                    m_scale), start * m_scale);
  oled->setScale(m_scale);
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
      m_selectedIndex = DISPLAY_HEIGHT / (SELECTED_HEIGHT *  m_scale) - 1;          // The last visible index
      m_scrollItemCount = static_cast<int8_t>(getSize() - 1 - m_selectedIndex);

      if (m_scrollItemCount < 0)
      {
        m_selectedIndex += m_scrollItemCount;
        m_scrollItemCount = 0;
      }
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
    if (nextIndex > getSize() - 1)
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
    if (nextIndex > getSize() - 1) return;
    ++m_scrollItemCount;
  }

  repaint();
}

ThreadSafeOLED *DisplayMenu::getSafeOled() const
{
  return m_safeOled;
}

vector<MenuItem> DisplayMenu::getItems() const
{
  return m_items;
}

bool DisplayMenu::getMenuLoop() const
{
  return m_menuLoop;
}

uint8_t DisplayMenu::getScale() const
{
  return m_scale;
}

size_t DisplayMenu::getUtf8Length(const String &str)
{
  size_t len = 0;

  for (size_t i = 0; i < str.length(); )
  {
    if (const uint8_t c = str[i]; c < 0x80) i += 1;           // ASCII character (1 byte)
    else if ((c & 0xE0) == 0xC0) i += 2;                      // 2-byte character
    else if ((c & 0xF0) == 0xE0) i += 3;                      // 3-byte character
    else i += 1;                                              // Invalid UTF-8
    len++;
  }

  return len;
}

String DisplayMenu::substringUtf8(const String &text, const size_t startCharIndex, const size_t endCharIndex)
{
  size_t startByteIndex = 0;
  size_t charCount = 0;

  while (charCount < startCharIndex && startByteIndex < text.length())
  {
    if (uint8_t c = text[startByteIndex]; c < 0x80) startByteIndex += 1;
    else if ((c & 0xE0) == 0xC0) startByteIndex += 2;
    else if ((c & 0xF0) == 0xE0) startByteIndex += 3;
    else startByteIndex += 1;
    charCount++;
  }

  size_t endByteIndex = 0;
  charCount = 0;

  while (charCount < endCharIndex && endByteIndex < text.length())
  {
    const uint8_t c = text[endByteIndex];
    const size_t charSize =
      c < 0x80 ? 1 :
      (c & 0xE0) == 0xC0 ? 2 :
      (c & 0xF0) == 0xE0 ? 3 : 1;

    if (charCount + 1 > endCharIndex) break;
    endByteIndex += charSize;
    charCount++;
  }

  return text.substring(startByteIndex, endByteIndex);
}

String DisplayMenu::getAvaliableString(const MenuItem &item, const bool selected, const uint16_t charIndex) const
{
  const String text = item.getText();
  String result;
  int8_t avaliableWidth = item.hasValue() ? DISPLAY_WIDTH - VALUE_RIGHT_OFFSET - VALUE_LEFT_OFFSET -
                          getUtf8Length(item.getValueAsString()) * CHAR_WIDTH * m_scale : DISPLAY_WIDTH;
  if (selected) avaliableWidth -= SELECTED_LEFT_OFFSET;
  if (avaliableWidth < CHAR_WIDTH * m_scale) return "";

  if (getUtf8Length(text) * CHAR_WIDTH * m_scale > avaliableWidth)
  {
    const uint8_t avaliableChar = avaliableWidth / (CHAR_WIDTH * m_scale) + charIndex;
    result = substringUtf8(text, charIndex, avaliableChar);

    if (getUtf8Length(result) + charIndex >= getUtf8Length(text))
    {
      result += substringUtf8(text, 0, avaliableChar - getUtf8Length(result) - charIndex);
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
  if (index != getSelectedIndex() || index >= getSize() || getSize()  == 0) return;
  const MenuItem item = getItem(index);
  if (charIndex + 1 ==  getUtf8Length(item.getText())) charIndex = 0;
  const String result = getAvaliableString(item, true, charIndex);
  if (result == item.getText() || result.isEmpty()) return;
  paintSelectedItem(result);
  this_thread::sleep_for(chrono::milliseconds(charIndex == 0 ? 1500 : 500));
  thread t(&DisplayMenu::scrollRight, this, index, charIndex + 1);
  t.detach();
}
