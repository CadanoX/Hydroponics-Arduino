#ifndef DS18B20_H
#define DS18B20_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20
{
public:
	DS18B20(int rx, int interval = 1000)
	{
		this->interval = interval;
		this->oneWire = new OneWire(rx);
		this->sensor = new DallasTemperature(oneWire);
		this->sensor->begin();
	}
	~DS18B20()
	{
		delete this->sensor;
		delete this->oneWire;
	}

	void read()
	{
		this->sensor->requestTemperatures();
		this->temperature = this->sensor->getTempCByIndex(0);
		this->newMeasurements = true;
	}

	void check(int currentTime = 0)
	{
		// if more time than interval was spent
		if (this->nextReadTime < currentTime)
		{
			this->read();
			this->nextReadTime = currentTime + this->interval;
		}
	}

	bool hasNewMeasurements()
	{
		return this->newMeasurements;
	}
	
	float getTemperature()
	{
		this->newMeasurements = false;
		return this->temperature;
	}

private:
	OneWire* oneWire;
	DallasTemperature* sensor;
	int nextReadTime = 0;
	int interval = 0;
	float temperature = -999;
	bool newMeasurements = false;
};

#endif