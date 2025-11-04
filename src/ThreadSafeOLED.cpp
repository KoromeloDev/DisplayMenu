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

ThreadSafeOLED::ThreadSafeOLED(GyverOLED<SSD1306_128x64> *oled): m_oled(oled)
{

}

ThreadSafeOLED::Lock ThreadSafeOLED::acquire()
{
  return Lock(*this);
}
