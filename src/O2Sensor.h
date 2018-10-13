#ifndef DOSENSOR_H
#define DOSENSOR_H

#include <Arduino.h>

class O2Sensor  
{
public:
	O2Sensor()
	{
		Serial1.begin(9600);
	}

	~O2Sensor()
	{

	}

	void read()
	{
		if (readline(Serial1.read(), this->serialReadBuffer, 80, this->readPos) > 0)
		{
			if (isdigit(*this->serialReadBuffer))
			{
				this->O2 = atof(this->serialReadBuffer);
				this->newMeasurements = true;
			}
		}
	}
	
	void write(const char* input)
	{
		Serial1.print(input); //send that string to the Atlas Scientific product
		Serial1.print('\r'); //add a <CR> to the end of the string
	}

	bool hasNewMeasurements()
	{
		return this->newMeasurements;
	}

	float getO2()
	{
		this->newMeasurements = false;
		return this->O2;
	}

private:
	bool newMeasurements = false;
	float O2 = -1;
	char serialReadBuffer[80];
	int readPos = 0;
};

#endif