/**
 * File: Timer.h
 * Description: This implements a timer
 */

#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:
  Timer(unsigned long ldelay, void (*lTimerFunc)());

  bool Update();

private:
  unsigned long lastUpdate;
  void (*timerFunc)();

  unsigned long delay; 
};
#endif
