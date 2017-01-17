#pragma once
#ifndef _Timer_h_
#define _Timer_h_

// Template on this class to get the template requirement out of the way 
// in cases where you don't want to use member function callbacks. 
class _Empty 
{}; 
 
// template this class on the class containing your timer callbacks .. 
// that is, if you want to use member function callbacks. 
template <typename callbackClass > 
class TimerManager : public Ogre::FrameListener 
{ 
public: 
  // UseFrameStarted true will cause timer callbacks to be sent 
  // during frame started.  False: callbacks will be sent 
  // during frame ended. 
  TimerManager( bool _UseFrameStarted = true ): UseFrameStarted( _UseFrameStarted ) 
  {} 
 
  // Two choices of callbacks. 
  // You can template this class on your class 
  // and use it to call member functions 
  // if your handler returns false, the timer will NOT be repeated. 
  typedef bool (callbackClass::*MemberTimerHandler)( int ); 
  // or you can provide a pointer to a static or global function 
  typedef bool (*TimerCallback)( int Timer_ID ); 
 
  // AddTimer() adds a timer to be called back after a given period 
  // Timer_ID is an optional identifier for later removal 
  // Repeat is a boolean flag, if true the timer handler will be called 
  // every Period untill stopped.  if false, timer handler will be called only once. 
  // Play_Catchup is a boolean flag, if true the timer handler will be called multiple times 
  // if TimerManager is unable to call at the requested rate due to low frame rate.    
  // the only difference between these two add functions is one uses member function callback 
  // the other uses a global callback. 
  void AddTimer( unsigned long Period, TimerCallback callback, int Timer_ID = -1, bool Repeat = false, 
                  bool Play_Catchup = true ) 
  { 
    TimerInstance t; 
    t.mtH = NULL; 
    t.thC = NULL; 
    t.tcb = callback; 
    t.Period = Period; 
    t.Timer_ID = Timer_ID; 
    t.Repeat = Repeat; 
    t.Play_Catchup = Play_Catchup; 
    unsigned long now = mTimer.getMilliseconds(); 
    timers.insert( std::make_pair(now + Period, t) ); 
  } 
  void AddTimer( unsigned long Period, MemberTimerHandler callback, callbackClass* callbackObject, 
                  int Timer_ID = -1, bool Repeat = false, bool Play_Catchup = true ) 
  { 
    TimerInstance t; 
    t.mtH = callback; 
    t.thC = callbackObject; 
    t.tcb = NULL; 
    t.Period = Period; 
    t.Timer_ID = Timer_ID; 
    t.Repeat = Repeat; 
    t.Play_Catchup = Play_Catchup; 
    unsigned long now = mTimer.getMilliseconds(); 
    timers.insert( std::make_pair(now + Period, t) ); 
  } 
 
  // searches for and removes all timers with given timer_ID 
  // note: If all the timers have the same ID, and that ID is prided here, it will end ALL timers 
  void StopTimer( int Timer_ID ) 
  { 
    TimerMap::iterator i; 
	std::list<TimerMap::iterator> its;
	//timers.erase(Timer_ID);
    for( i = timers.begin(); i != timers.end(); ++i ) 
    { 
      if( i->second.Timer_ID == Timer_ID ) 
      { 
		  its.push_back(i);
        //timers.erase(i); 
        // we have to start our search over since erase invalidates our iterator 
		//i = timers.begin();
      } 
    } 
	for (std::list<TimerMap::iterator>::iterator j = its.begin(); j != its.end(); ++j) {
		timers.erase(*j);
	}
  } 
protected: 
  // Ogre::Timer to watch time pass by. 
  Ogre::Timer mTimer; 
  // definition of our timer instance. 
  class TimerInstance 
  { 
  public: 
    MemberTimerHandler mtH; 
    callbackClass* thC; 
    TimerCallback     tcb; 
    unsigned long Period; 
    int Timer_ID; 
    bool Repeat; 
    bool Play_Catchup; 
    unsigned long Trigger(unsigned long behind) 
    { 
      bool callback_return = false; 
      long temp_behind = (long)behind; 
      while( temp_behind >= 0 ) 
      { 
        temp_behind -= Period; 
        callback_return = Callback(); 
        if( !Repeat ) 
          return 0; 
        if( !callback_return ) 
          return 0; 
        if( Period == 0 ) 
          return 0; 
        if( !Play_Catchup ) 
          continue; 
      } 
 
      return (Period - (behind % Period)); 
    } 
    bool Callback() 
    { 
      if( ( mtH != NULL ) && (thC != NULL ) ) 
      { 
        return (thC->*mtH)(Timer_ID); 
      } 
      else if ( tcb != NULL ) 
      { 
        return tcb(Timer_ID); 
      } 
      return false; 
    } 
  }; 
  // inherited frameStarted and frame Ended from Ogre::FrameListener 
  virtual bool frameStarted(const Ogre::FrameEvent& evt) 
  { 
    if( !UseFrameStarted ) 
      return true; 
    while(Next()); 
    return true; 
  } 
  virtual bool frameEnded(const Ogre::FrameEvent& evt) 
  { 
    if( UseFrameStarted ) 
      return true; 
    while(Next()); 
    return true; 
  } 
  // process next timer (if applicable) 
  bool Next() 
  { 
    if ( timers.empty() ) 
      return false; 
    unsigned long now = mTimer.getMilliseconds(); 
    unsigned long then = timers.begin()->first; 
    //std::cout << " now >= then " << now << " >= " << then << std::endl; 
    if( now >= then) 
    { 
      TimerInstance currentTimer = timers.begin()->second; // get a copy of the timer instance 
      timers.erase(timers.begin()); // pop the sucker off the map. 
      unsigned long next_trigger = currentTimer.Trigger( now - then ); // trigger it 
      if( next_trigger > 0 ) 
      { 
        // if it returns non-zero, put it back in the map. 
        timers.insert(std::make_pair( now + next_trigger, currentTimer )); 
      } 
      return true; 
    } 
    return false; 
  } 
  // container for all our timer instances. 
  typedef std::multimap< int, TimerInstance > TimerMap; 
  std::multimap< int, TimerInstance > timers; 
  // determines if callbacks happen durring frame started, or frame ended. 
  bool UseFrameStarted; 
};

#endif