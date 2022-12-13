// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice 
//      if we're currently running something (in other words, not idle).
//	Also, to keep from looping forever, we check if there's
//	nothing on the ready list, and there are no other pending
//	interrupts.  In this case, we can safely halt.
//----------------------------------------------------------------------

void 
Alarm::CallBack() 
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();

    // add 11-15
    bool wake = _Sleep_pool.wakeup() ; 
    
    if (status == IdleMode && !wake && _Sleep_pool.pool_empty()) {	// is it time to quit?
        if (!interrupt->AnyFutureInterrupts()) {
	    timer->Disable();	// turn off the timer
	}
    } else {			// there's someone to preempt
	interrupt->YieldOnReturn();
    }
}

void Alarm::WaitUntil( int x ) 
{
    IntStatus previous_level = kernel->interrupt->SetLevel(IntOff) ;

    Thread* t = kernel->currentThread ; 

    cout << "Alarm WaitUntil start sleep " << endl ; 

    _Sleep_pool.add2sleep(t,x)    ; 

    kernel->interrupt->SetLevel(previous_level) ; 
}


void Sleep_pool::add2sleep(Thread *t, int x ) 
{
    ASSERT(kernel ->interrupt->getLevel() == IntOff ) ; 

    _Sleep_pool.push_back( Sleep_thread(t , _current_interrupt + x )) ; 

    t -> Sleep(false) ; 
} 


bool Sleep_pool::pool_empty() 
{
    return _Sleep_pool.size() == 0 ; 
} 


bool Sleep_pool::wakeup() 
{
    bool wake = false ; 

    _current_interrupt ++ ; 

    for( std::list<Sleep_thread>::iterator iter = _Sleep_pool.begin() ; iter != _Sleep_pool.end(); )
    {
        if (_current_interrupt >= iter->when) 
        {
            wake = true ; 
            cout << "Thread wakeup" << endl ; 
            kernel -> scheduler -> ReadyToRun(iter->sleep) ; 
            iter = _Sleep_pool.erase(iter) ; 
        }
        else 
        {
            iter ++ ; 
        }
    }

    return wake ; 
}