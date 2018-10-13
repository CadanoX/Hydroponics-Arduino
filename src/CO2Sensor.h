#ifndef CO2SENSOR_H
#define CO2ENSOR_H

#include <Arduino.h>

class CO2Sensor  
{
public:
	CO2Sensor()
	{
		Serial1.begin(9600);
	}

	~CO2Sensor()
	{

	}

	void read()
	{
		if (readline(Serial1.read(), this->serialReadBuffer, 80, this->readPos) > 0)
		{
			if (isdigit(*this->serialReadBuffer))
			{
				this->CO2 = atof(this->serialReadBuffer);
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

	float getCO2()
	{
		this->newMeasurements = false;
		return this->CO2;
	}

private:
	bool newMeasurements = false;
	float CO2 = -1;
	char serialReadBuffer[80];
	int readPos = 0;
};

#endif