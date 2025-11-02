# DisplayMenu
DisplayMenu is a library based on the GyverOLED library for easily creating menus for SSD1306 128x64 displays.

## Why don't you use GyverOLED MENU?
GyverOLED MENU does not support dynamic creation and deletion of menu items, unlike this library.

## What features are currently available?
- [x] Dynamic addition/removal of items
- [x] Navigation through menu items
- [x] Cyclical transition between the first and last items (can be disabled)
- [x] Correct display of elements that do not fit vertically on the screen
- [x] Scroll down/up
- [x] Display of item values
- [x] Changes to item values

## It is planned to do
- [ ] Correct display of elements that do not fit horizontally on the screen

## Documentation
### Initialization
```cpp
#include <DisplayMenu.h>

GyverOLED<SSD1306_128x64> oled;
DisplayMenu menu(&oled);

void setup()
{
  menu.setup();
  // You can add elements
}
```

### Usage
```cpp
void addItem(const String &text)                  // Adding an element without value
void addItem(const String &text, &value)          // Adding an element with a value, where “&value” is a reference to a value of type: bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, float, double
void deleteItem(uint8_t index);                   // Removing an element by index

void selectNext();                                // Navigates to the next menu index
void selectPrevious();                            // Navigates to the зrevious menu index
void setMenuLoop(bool enable);                    // If enabled (default): when selectNext() is called on the last element, it switches to the first element, and in reverse when selectPrevious() is called.

void incrementValue() const;                      // Increases 1 to the current value. If the value is bool, it inverts it.
void decrementValue() const;                      // Decreases 1 to the current value. If the value is bool, it inverts it.

void repaint() const;                             // Repaints the menu

[[nodiscard]] MenuItem getItem(uint8_t index);    // Returns an item by index
[[nodiscard]] MenuItem getSelectedItem() const;   // Returns the current selected item
[[nodiscard]] uint8_t  getSelectedIndex() const;  // Returns the current selected index
[[nodiscard]] String   getSelectedText() const;   // Returns the current selected text
```
