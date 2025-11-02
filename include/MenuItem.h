#ifndef MENUITEM_H
#define MENUITEM_H

#include <optional>
#include <variant>
#include <type_traits>
#include <WString.h>

using namespace std;

class MenuItem
{
public:
  using ValueType = variant<bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t,
                    float, double>;
  using ValuePtrType = variant<bool*, uint8_t*, int8_t*, uint16_t*, int16_t*, uint32_t*, int32_t*, uint64_t*, int64_t*,
                       float*, double*>;

  explicit MenuItem(String text);
  template <typename PTR_T>
  MenuItem(String  text, PTR_T value) : m_text(std::move(text)), m_value(optional<PTR_T>(value))
  {
    checkType(value);
  }
  template <typename PTR_T, typename T>
  MenuItem(String  text, PTR_T value, T min) : m_text(std::move(text)), m_value(optional<PTR_T>(value)),
  m_valueMin(optional<T>(min))
  {
    checkType(value);
    checkType(min);
  }
  template <typename PTR_T, typename T>
  MenuItem(String  text, PTR_T value, T min, T max) : m_text(std::move(text)), m_value(optional<PTR_T>(value)),
  m_valueMin(optional<T>(min)), m_valueMax(optional<T>(max))
  {
    checkType(value);
    checkType(min);
  }

  [[nodiscard]] String getText() const;

  [[nodiscard]] bool hasValue() const;
  [[nodiscard]] bool hasMin() const;
  [[nodiscard]] bool hasMax() const;

  [[nodiscard]] optional<ValuePtrType> getValue() const;
  [[nodiscard]] optional<ValueType> getMin() const;
  [[nodiscard]] optional<ValueType> getMax() const;
  [[nodiscard]] String getValueAsString() const;

  void incrementValue();
  void decrementValue();

private:
  String m_text;
  optional<ValuePtrType> m_value;
  optional<ValueType> m_valueMin;
  optional<ValueType> m_valueMax;

  void trimValue() const;
  void modifyValue(bool increment);

  template<typename T>
  constexpr void checkType(T)
  {
    static_assert(numeric_limits<remove_pointer_t<T>>::is_bounded, "Unsupported type for value");
  }
};

#endif // MENUITEM_H