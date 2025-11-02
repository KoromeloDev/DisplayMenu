#include "MenuItem.h"

#define DECIMAL_PLACES 2

MenuItem::MenuItem(String text) : m_text(std::move(text))
{
  m_value = nullopt;
  m_valueMin = nullopt;
  m_valueMax = nullopt;
}

String MenuItem::getText() const
{
  return m_text;
}

bool MenuItem::hasValue() const
{
  return m_value.has_value();
}

bool MenuItem::hasMin() const
{
  return m_valueMin.has_value();
}

bool MenuItem::hasMax() const
{
  return m_valueMax.has_value();
}

optional<MenuItem::ValuePtrType> MenuItem::getValue() const
{
  return m_value;
}

optional<MenuItem::ValueType> MenuItem::getMin() const
{
  return m_valueMin;
}

optional<MenuItem::ValueType> MenuItem::getMax() const
{
  return m_valueMax;
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
    else if constexpr (is_floating_point_v<T*>)
    {
      return String(*arg, DECIMAL_PLACES);
    }
    else
    {
      return String(*arg); // For integer types
    }
  }, m_value.value());
}

void MenuItem::trimValue() const
{
  if (hasMin())
  {
    auto trimMin = [](auto &&ptr, auto &&minVal)
    {
      if (*ptr < minVal) *ptr = minVal;
    };

    visit(trimMin, m_value.value(), m_valueMin.value());
  }

  if (hasMax())
  {
    auto trimMin = [](auto &&ptr, auto &&maxVal)
    {
      if (*ptr > maxVal) *ptr = maxVal;
    };

    visit(trimMin, m_value.value(), m_valueMax.value());
  }
}

void MenuItem::modifyValue(bool increment)
{
  if (!hasValue()) return;

  visit([&increment](auto &&arg)
  {
    using T = decay_t<decltype(arg)>;

    if constexpr (is_same_v<T, bool>)
    {
      *arg = !*arg;
    }
    else
    {
      *arg += increment ? 1 : -1;
    }
  }, m_value.value());

  trimValue();
}
