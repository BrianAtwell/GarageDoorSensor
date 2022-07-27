/**
 * File: Timer.cpp
 * Description: This implements a timer
 */

#include "Timer.h"
#include <Arduino.h>

Timer::Timer(unsigned long ldelay, void (*lTimerFunc)()) : delay(ldelay), timerFunc(lTimerFunc), lastUpdate(millis()){}

bool Timer::Update()
{
  unsigned long timediff = millis()- lastUpdate;

  if(timediff > delay)
  {
    if(timerFunc != nullptr)
    {
      timerFunc();
    }
    lastUpdate = millis();
    return true;
  }

  return false;
}

void Timer::Reset()
{
  lastUpdate=millis();
}
