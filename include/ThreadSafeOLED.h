#ifndef THREADSAFEOLED_H
#define THREADSAFEOLED_H

#include <GyverOLED.h>
#include <mutex>

using namespace std;

class ThreadSafeOLED
{
public:
  class Lock
  {
  public:
    explicit Lock(ThreadSafeOLED &ts_oled);

    GyverOLED<SSD1306_128x64> *operator->() const;
    GyverOLED<SSD1306_128x64> &operator*() const;
    void lock();
    void unlock();

  private:
    GyverOLED<SSD1306_128x64> *m_oled;
    std::unique_lock<std::mutex> m_lock;
  };

  ThreadSafeOLED();
  ~ThreadSafeOLED();

  Lock acquire();

private:
  friend class Lock;
  GyverOLED<SSD1306_128x64> *m_oled;
  std::mutex mutex;
};

#endif //THREADSAFEOLED_H