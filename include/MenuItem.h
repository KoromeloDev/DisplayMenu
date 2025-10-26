#pragma once

#include <variant>
#include <optional>
#include <WString.h>

#define CHECK_TYPE_ASSERT static_assert( \
        is_same_v<T, bool> || \
        is_same_v<T, uint8_t> || \
        is_same_v<T, int8_t> || \
        is_same_v<T, uint16_t> || \
        is_same_v<T, int16_t> || \
        is_same_v<T, uint32_t> || \
        is_same_v<T, int32_t> || \
        is_same_v<T, float> || \
        is_same_v<T, double>, \
        "Unsupported type for value");

#define CHECK_TYPE_ASSERT_PTR static_assert( \
        is_same_v<PTR_T, bool*> || \
        is_same_v<PTR_T, uint8_t*> || \
        is_same_v<PTR_T, int8_t*> || \
        is_same_v<PTR_T, uint16_t*> || \
        is_same_v<PTR_T, int16_t*> || \
        is_same_v<PTR_T, uint32_t*> || \
        is_same_v<PTR_T, int32_t*> || \
        is_same_v<PTR_T, float*> || \
        is_same_v<PTR_T, double*>, \
        "Unsupported type point for value");

using namespace std;

class MenuItem
{
public:
  using ValueType = variant<bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, float, double>;
  using ValuePtrType = variant<bool*, uint8_t*, int8_t*, uint16_t*, int16_t*, uint32_t*, int32_t*, float*, double*>;

  explicit MenuItem(String text);
  template <typename PTR_T>
  MenuItem(String  text, PTR_T value) : _text(std::move(text)), _value(optional<ValuePtrType>(value))
  {
    CHECK_TYPE_ASSERT_PTR
  }
  template <typename PTR_T, typename T>
  MenuItem(String  text, PTR_T value, T min) : _text(std::move(text)), _value(optional<ValuePtrType>(value)),
  _valueMin(optional<ValueType>(min))
  {
    CHECK_TYPE_ASSERT_PTR
    CHECK_TYPE_ASSERT
  }
  template <typename PTR_T, typename T>
  MenuItem(String  text, PTR_T value, T min, T max) : _text(std::move(text)), _value(optional<ValuePtrType>(value)),
  _valueMin(optional<ValueType>(min)), _valueMax(optional<ValueType>(max))
  {
    CHECK_TYPE_ASSERT_PTR
    CHECK_TYPE_ASSERT
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
  String _text;
  optional<ValuePtrType> _value;
  optional<ValueType> _valueMin;
  optional<ValueType> _valueMax;

  void trimValue() const;
  void modifyValue(bool increment);
};
