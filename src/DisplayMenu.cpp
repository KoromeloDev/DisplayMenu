#include "DisplayMenu.h"

#define SELECTED_HEIGHT 10
#define SELECTED_LEFT_OFFSET 8
#define VALUE_RIGHT_OFFSET 0
#define CHAR_WIDTH 6

DisplayMenu::DisplayMenu(GyverOLED<SSD1306_128x64> *oled) : _oled(oled)
{
  _selectedIndex = 0;
  _scrollItemCount = 0;
  _menuLoop = true;
}

void DisplayMenu::setup() const
{
  _oled->init();
  Wire.setClock(800000L);
  _oled->clear();
  _oled->update();
}

void DisplayMenu::setMenuLoop(const bool enable)
{
  _menuLoop = enable;
}

void DisplayMenu::selectNext()
{
  if (_items.empty()) return;

  if ((_selectedIndex + 1) * SELECTED_HEIGHT + SELECTED_HEIGHT > 63)
  {
    scrollDown();
    return;
  }

  const uint8_t index = _items.size() - 1;

  if (_menuLoop)
  {
      unselectIndex(_selectedIndex);
      _selectedIndex = _selectedIndex == index ? 0 : ++_selectedIndex;
  }
  else
  {
    if (_selectedIndex == index) return;
    unselectIndex(_selectedIndex);
    ++_selectedIndex;
  }

  selectIndex();
  _oled->update();
}

void DisplayMenu::selectPrevious()
{
  if (_items.empty()) return;

  if (_selectedIndex == 0)
  {
    scrollUp();
    return;
  }

  const uint8_t index = _items.size() - 1;

  if (_menuLoop)
  {
    unselectIndex(_selectedIndex);
    _selectedIndex = _selectedIndex == 0 ? index : --_selectedIndex;
  }
  else
  {
    if (_selectedIndex == 0) return;
    unselectIndex(_selectedIndex);
    --_selectedIndex;
  }

  selectIndex();
  _oled->update();
}

MenuItem DisplayMenu::getSelectedItem() const
{
  return _items.at(getSelectedIndex());
}

void DisplayMenu::addItem(const String &text)
{
  _items.emplace_back(text);
  addItem();
}

void DisplayMenu::incrementValue() const
{
  if (_items.empty()) return;
  getSelectedItem().incrementValue();
  selectIndex();
  _oled->update();
}

void DisplayMenu::decrementValue() const
{
  if (_items.empty()) return;
  getSelectedItem().decrementValue();
  selectIndex();
  _oled->update();
}

MenuItem DisplayMenu::getItem(const uint8_t index)
{
  return _items.at(index);
}

uint8_t DisplayMenu::getSelectedIndex() const
{
  return _selectedIndex + _scrollItemCount;
}

String DisplayMenu::getSelectedText() const
{
  return _items.at(getSelectedIndex()).getText();
}

void DisplayMenu::scrollUp()
{
  if (_items.empty()) return;
  const auto nextIndex = static_cast<int8_t>(_selectedIndex + _scrollItemCount - 1);

  if (_menuLoop)
  {
    if (nextIndex == -1)
    {
      _selectedIndex = 5;                             // The last visible index
      _scrollItemCount = static_cast<int8_t>(_items.size() - 1 - _selectedIndex);
    }
    else
    {
      --_scrollItemCount;
    }
  }
  else
  {
    if (nextIndex == -1) return;
    --_scrollItemCount;
  }

  repaint();
}

void DisplayMenu::scrollDown()
{
  if (_items.empty()) return;
  const auto nextIndex = static_cast<int8_t>(_selectedIndex + _scrollItemCount + 1);

  if (_menuLoop)
  {
    if (nextIndex > _items.size() - 1)
    {
      _scrollItemCount = 0;
      _selectedIndex = 0;
    }
    else
    {
      ++_scrollItemCount;
    }
  }
  else
  {
    if (nextIndex > _items.size() - 1) return;
    ++_scrollItemCount;
  }

  repaint();
}

void DisplayMenu::deleteItem(const uint8_t index)
{
  if (index >= _items.size()) return;
  _items.erase(_items.begin() + index);

  if (index == getSelectedIndex() || getSelectedIndex() >= _items.size())
  {
    if (_scrollItemCount > 0)
    {
      scrollUp();
      return;
    }

    if (_selectedIndex > 0)
    {
      --_selectedIndex;
    }
  }

  _oled->clear();
  repaint();
}

void DisplayMenu::repaint() const
{
  const uint8_t sizeToPrint = _items.size() >= 6 ? 6 : _items.size();

  for (uint8_t i = 0; i < sizeToPrint; ++i)
  {
    if (i == _selectedIndex) selectIndex();
    else unselectIndex(i);
  }

  _oled->update();
}

void DisplayMenu::addItem() const
{
  if (const uint8_t index = _items.size() - 1; index == _selectedIndex)
  {
    selectIndex();
  }
  else
  {
    unselectIndex(index);
  }

  _oled->update();
}

void DisplayMenu::valuePrint(const MenuItem &item, const uint8_t start) const
{
  if (!item.hasValue()) return;
  const String valueString = item.getValueAsString();
  _oled->setCursorXY(static_cast<uint8_t>(127 - VALUE_RIGHT_OFFSET - valueString.length() * CHAR_WIDTH), start);
  _oled->print(valueString);
}

void DisplayMenu::selectIndex() const
{
  if (_items.empty()) return;

  const uint8_t start = _selectedIndex * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 2 >= 63 ? 63 : start + SELECTED_HEIGHT - 2;
  _oled->rect(0, start, 128, end, OLED_FILL);
  _oled->setCursorXY(SELECTED_LEFT_OFFSET, start + 1);
  _oled->textMode(BUF_SUBTRACT);
  _oled->print(getSelectedText());
  valuePrint(getSelectedItem(), start + 1);
}

void DisplayMenu::unselectIndex(const uint8_t index) const
{
  const uint8_t start = index * SELECTED_HEIGHT;
  const uint8_t end = start + SELECTED_HEIGHT - 1 >= 63 ? 63 : start + SELECTED_HEIGHT - 1;
  const MenuItem item = _items.at(index + _scrollItemCount);
  _oled->clear(0, start, 128, end);
  _oled->setCursorXY(0, start);
  _oled->textMode(BUF_ADD);
  _oled->print(item.getText());
  valuePrint(item, start);
}
