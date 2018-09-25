#ifndef PUMP_H
#define PUMP_H

#include "Arduino.h"

//class describing a pump
class Pump                                           
{
public:
	//constructor sets pin, on which the pump is located
	Pump(int pin, int pin2 = -1) : pin(pin), pin2(pin2)
	{
		//set pump pins as output to be able to write to it
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
		if (pin2 != -1)
		{
			pinMode(pin2, OUTPUT);
			digitalWrite(pin2, LOW);
		}
	}

	//let the pump run for "runtime" milliseconds and then stop
	void start(int runtime = -1)                              
	{
		digitalWrite(pin, HIGH);
	}
	
	void stop()
	{
		digitalWrite(pin, LOW);
	}

private:
	int pin;
	int pin2;
};

#endif