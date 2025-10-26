#include "MenuItem.h"

#define DECIMAL_PLACES 2

MenuItem::MenuItem(String text) : _text(std::move(text))
{
  _value = nullopt;
  _valueMin = nullopt;
  _valueMax = nullopt;
}

String MenuItem::getText() const
{
  return _text;
}

bool MenuItem::hasValue() const
{
  return _value.has_value();
}

bool MenuItem::hasMin() const
{
  return _valueMin.has_value();
}

bool MenuItem::hasMax() const
{
  return _valueMax.has_value();
}

optional<MenuItem::ValuePtrType> MenuItem::getValue() const
{
  return _value;
}

optional<MenuItem::ValueType> MenuItem::getMin() const
{
  return _valueMin;
}

optional<MenuItem::ValueType> MenuItem::getMax() const
{
  return _valueMax;
}

void MenuItem::incrementValue()
{
  modifyValue(true);
}

void MenuItem::decrementValue()
{
  modifyValue(false);
}

String MenuItem::getValueAsString() const
{
  if (!hasValue()) return "";

  return visit([](auto &&arg) -> String
  {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, bool*>)
    {
      return *arg ? "true" : "false";
    }
    else if constexpr (is_same_v<T, float*> || is_same_v<T, double*>)
    {
      return String(*arg, DECIMAL_PLACES);
    }
    else
    {
      return String(*arg); // For integer types
    }
  }, _value.value());
}

void MenuItem::trimValue() const
{
  if (hasMin())
  {
    auto trimMin = [](auto &&ptr, auto &&minVal)
    {
      if (*ptr < minVal) *ptr = minVal;
    };

    visit(trimMin, _value.value(), _valueMin.value());
  }

  if (hasMax())
  {
    auto trimMin = [](auto &&ptr, auto &&maxVal)
    {
      if (*ptr > maxVal) *ptr = maxVal;
    };

    visit(trimMin, _value.value(), _valueMax.value());
  }
}

void MenuItem::modifyValue(bool increment)
{
  if (!hasValue()) return;

  visit([&increment](auto &&arg)
  {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, bool*>)
    {
      *arg = !*arg;
    }
    else
    {
      *arg += increment ? 1: -1;
    }
  }, _value.value());

  trimValue();
}
