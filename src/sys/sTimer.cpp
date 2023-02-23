
#include <string.h>
#if defined(QT_WIDGETS_LIB)
#include <sys/time.h>
#include <stdint.h>
#endif
#include "sTimer.h"


#if defined(QT_WIDGETS_LIB)
static uint64_t millis() {
    struct timeval    now;
    gettimeofday(&now, NULL);
    return ((uint64_t)now.tv_sec * 1000000 + (uint64_t)now.tv_usec) / 1000;
}
#endif

sTimer::sTimer() {
	memset(this, 0, sizeof(this));
}

sTimer::sTimer(uint64_t interval) {
	prevTime = 0;
	enabled = false;
	this->interval = interval;
}

sTimer::~sTimer() {
    if(!this)
        return;
    memset(this, 0, sizeof(this));
}

void sTimer::SetInterval(uint64_t interval) {
    if(!this)
        return;
    this->interval = interval;
	this->prevTime = millis();
}

void sTimer::Start(uint64_t interval) {
    if(!this)
        return;
    this->interval = interval;
	Start();
}

void sTimer::Start() {
    if(!this)
        return;
    this->prevTime = millis();
	this->enabled = true;
}

void sTimer::Stop() {
    if(!this)
        return;
    this->enabled = false;
}

bool sTimer::Tick() {
    if(!this)
        return false;
	if(this->enabled == false)
		return false;
	if(millis() >= this->prevTime + this->interval) {
		this->prevTime = millis();
		return true;
	}
	return false;
}
