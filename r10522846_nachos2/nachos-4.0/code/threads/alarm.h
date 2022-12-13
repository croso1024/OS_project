// alarm.h 
//	Data structures for a software alarm clock.
//
//	We make use of a hardware timer device, that generates
//	an interrupt every X time ticks (on real systems, X is
//	usually between 0.25 - 10 milliseconds).
//
//	From this, we provide the ability for a thread to be
//	woken up after a delay; we also provide time-slicing.
//
//	NOTE: this abstraction is not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ALARM_H
#define ALARM_H

#include "copyright.h"
#include "utility.h"
#include "callback.h"
#include "timer.h"

// 11-15 add 
#include "thread.h" 
#include <list> 

class Sleep_pool 
{
    public: 
        Sleep_pool():_current_interrupt(0) {}; 
        void add2sleep(Thread *t , int x) ; 
      bool wakeup() ; 
      bool pool_empty(); 

    private:  

      class Sleep_thread 
      {
        public : 
          Sleep_thread(Thread *t , int x ) : 
          sleep(t) , when(x) {} ; 
          Thread *sleep ; 
          int when  ; 
      };

    int _current_interrupt ; 
    std::list<Sleep_thread> _Sleep_pool ; 
} ; 

// The following class defines a software alarm clock. 
class Alarm : public CallBackObj {
  public:
    Alarm(bool doRandomYield);	// Initialize the timer, and callback 
				// to "toCall" every time slice.
    ~Alarm() { delete timer; }
    
    void WaitUntil(int x);	// suspend execution until time > now + x

  private:
    Timer *timer;		// the hardware timer device

    // add 11-15
    Sleep_pool  _Sleep_pool ; 

    void CallBack();		// called when the hardware
				// timer generates an interrupt
};

#endif // ALARM_H
