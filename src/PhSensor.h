#ifndef PHSENSOR_H
#define PHSENSOR_H

#include <Arduino.h>

class PhSensor  
{
public:
	PhSensor()
	{
		Serial3.begin(9600);
	}

	~PhSensor()
	{

	}

	void read()
	{
		if (readline(Serial3.read(), this->serialReadBuffer, 80, this->readPos) > 0)
		{
			if (isdigit(*this->serialReadBuffer))
			{
				this->PH = atof(this->serialReadBuffer);
				this->newMeasurements = true;
			}
		}
	}
	
	void write(const char* input)
	{
		Serial3.print(input); //send that string to the Atlas Scientific product
		Serial3.print('\r'); //add a <CR> to the end of the string
	}

	bool hasNewMeasurements()
	{
		return this->newMeasurements;
	}

	float getPh()
	{
		this->newMeasurements = false;
		return this->PH;
	}

private:
	bool newMeasurements = false;
	float PH = -1;
	char serialReadBuffer[80];
	int readPos = 0;
};

#endif