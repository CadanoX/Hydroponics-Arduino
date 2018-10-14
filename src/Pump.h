#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>

class Pump                                           
{
public:
	Pump(int pin, int pin2 = -1) :
		pin(pin),
		pin2(pin2)
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
		if (pin2 != -1)
		{
			pinMode(pin2, OUTPUT);
			digitalWrite(pin2, LOW);
		}
	}

	~Pump()
	{
		this->stop();
	}

	void start(unsigned long timeToStop = 0)                              
	{
		if (this->state == IDLE)
		{
			digitalWrite(this->pin, HIGH);
			if (timeToStop == 0)
				this->state = RUNNING;
			else {
				this->state = RUNNING_ON_TIMER;
				this->timeToStop = timeToStop;
			}
				
		}
	}
	
	void stop()
	{
		if (this->state != IDLE)
		{
			digitalWrite(this->pin, LOW);
			this->state = IDLE;
		}
	}

	void check(unsigned long currentTime)
	{
		if (this->state == RUNNING_ON_TIMER)
			if (this->timeToStop < currentTime)
				this->stop();
	}

private:
	enum State {
		IDLE,
		RUNNING,
		RUNNING_ON_TIMER
	} state;
	int pin;
	int pin2;
	unsigned long timeToStop = 0;
};

#endif