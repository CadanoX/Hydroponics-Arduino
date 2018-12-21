#ifndef ECSENSOR_H
#define ECSENSOR_H

#include <Arduino.h>

class EcSensor                                           
{
public:
	EcSensor()
	{
		Serial1.begin(9600);
	}

	~EcSensor()
	{

	}

	void read()
	{
		if (readline(Serial1.read(), this->serialReadBuffer, 80, this->readPos) > 0)
		{
			if (isdigit(*this->serialReadBuffer))
			{
				// parse the array at each comma
				char* sEC = strtok(this->serialReadBuffer, ",");
				char* sTDS = strtok(NULL, ",");
				char* sSAL = strtok(NULL, ",");
				char* sGRAV = strtok(NULL, ",");

				// convert string to float
				this->EC = atof(sEC);
				this->TDS = atof(sTDS);
				this->SAL = atof(sSAL);
				this->GRAV = atof(sGRAV);
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

	float getEc()
	{
		this->newMeasurements = false;
		return this->EC;
	}

    float getTds()
	{
		this->newMeasurements = false;
		return this->TDS;
	}

    float getSal()
	{
		this->newMeasurements = false;
		return this->SAL;
	}

    float getGrav()
	{
		this->newMeasurements = false;
		return this->GRAV;
	}

private:
	bool newMeasurements = false;
	float EC = -1;
	float TDS = -1; 
	float SAL = -1; 
	float GRAV = -1;
	char serialReadBuffer[80];
	int readPos = 0;
};

#endif