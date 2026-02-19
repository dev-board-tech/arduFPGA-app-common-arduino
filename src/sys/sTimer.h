#ifndef __TIMER__
#define __TIMER__

#if !defined(QT_WIDGETS_LIB)
#include "Arduino.h"
#endif
#include <stdbool.h>
#include <stdint.h>

class sTimer {
public:
	sTimer();
    sTimer(uint64_t interval);
	~sTimer();
    void SetInterval(uint64_t interval);
    void Start(uint64_t interval);
	void Start();
	void Stop();
	bool Tick();
	bool Enabled();
private:
    uint64_t interval;
    uint64_t prevTime;
	bool enabled;
};

#endif

