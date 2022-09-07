#ifndef __TIMER__
#define __TIMER__

#include "Arduino.h"

#include "stdbool.h"

class sTimer {
public:
	sTimer();
	sTimer(long interval);
	~sTimer();
	void SetInterval(long interval);
	void Start(long interval);
	void Start();
	void Stop();
	bool Tick();
private:
	long interval;
	long prevTime;
	bool enabled;
};

#endif

