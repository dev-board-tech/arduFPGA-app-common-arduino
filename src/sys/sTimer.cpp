
#include "Arduino.h"

#include <string.h>
#include "sTimer.h"

sTimer::sTimer() {
	memset(this, 0, sizeof(this));
}

sTimer::sTimer(long interval) {
	prevTime = 0;
	enabled = false;
	this->interval = interval;
}

sTimer::~sTimer() {
	memset(this, 0, sizeof(this));
}

void sTimer::SetInterval(long interval) {
	this->interval = interval;
	this->prevTime = millis();
}

void sTimer::Start(long interval) {
	this->interval = interval;
	Start();
}

void sTimer::Start() {
	this->prevTime = millis();
	this->enabled = true;
}

void sTimer::Stop() {
	this->enabled = false;
}

bool sTimer::Tick() {
	if(this->enabled == false)
		return false;
	if(millis() >= this->prevTime + this->interval) {
		this->prevTime = millis();
		return true;
	}
	return false;
}
