#include "ThreadSafeOLED.h"

ThreadSafeOLED::Lock::Lock(ThreadSafeOLED &ts_oled): m_oled(ts_oled.m_oled), m_lock(ts_oled.mutex)
{

}

GyverOLED<SSD1306_128x64> *ThreadSafeOLED::Lock::operator->() const
{
  return m_oled;
}

GyverOLED<SSD1306_128x64> &ThreadSafeOLED::Lock::operator*() const
{
  return *m_oled;
}

void ThreadSafeOLED::Lock::lock()
{
  m_lock.lock();
}

void ThreadSafeOLED::Lock::unlock()
{
  m_lock.unlock();
}

ThreadSafeOLED::ThreadSafeOLED()
{
  m_oled = new GyverOLED<SSD1306_128x64>();
}

ThreadSafeOLED::~ThreadSafeOLED()
{
  delete m_oled;
  m_oled = nullptr;
}

ThreadSafeOLED::Lock ThreadSafeOLED::get()
{
  return Lock(*this);
}
