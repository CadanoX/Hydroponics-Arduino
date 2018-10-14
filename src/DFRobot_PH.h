#ifndef DFROBOT_PH_H
#define DFROBOT_PH_H

#define PH_NUM_SAMPLES 10

#include <Arduino.h>

class DFRobot_PH
{
public:
	DFRobot_PH(int pin, int interval = 1000, int samplingRate = 80) :
		pin(pin),
		interval(interval),
		samplingRate(samplingRate)
	{
		this->initSampleValues();
	}

	~DFRobot_PH()
	{
	}

	void read()
	{
		this->PH = 3.5 * avgSampleValues() * voltageCorrection;
		this->newMeasurements = true;
	}

	void check(unsigned long currentTime = 0)
	{
		// sample, if more time than samplingTime was spent
		if (this->nextSampleTime < currentTime)
		{
			this->sample();
			this->nextSampleTime = currentTime + this->samplingRate;
		}

		// read, if more time than interval was spent
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

	float getPh()
	{
		this->newMeasurements = false;
		return this->PH;
	}

private:

	void sample()
	{
		sampleValues[sampleIndex++] = analogRead(this->pin);
		if(sampleIndex == PH_NUM_SAMPLES)
			sampleIndex = 0;
	}

	void initSampleValues()
	{
		sampleIndex = -1;
		for (int i = 0; i < PH_NUM_SAMPLES; i++)
			sample();
	}

	// average all sampled values except the lowest and highest
	double avgSampleValues()
	{
		int min, max;
		long sum = 0;

		// initialize min and max values with the first 2 values
		if (sampleValues[0] < sampleValues[1]) {
			min = sampleValues[0];
			max = sampleValues[1];
		}
		else {
			min = sampleValues[0];
			max = sampleValues[1];
		}

		// only add the value to the sum, if it is neither the min, nor max value
		for (int i = 2; i < PH_NUM_SAMPLES; i++)
		{
			if (sampleValues[i] < min) {
				sum += min;
				min = sampleValues[i];
			}
			else if (sampleValues[i] > max) {
				sum += max;
				max = sampleValues[i];
			}
			else
				sum += sampleValues[i];
		}

		return (double)sum / (PH_NUM_SAMPLES - 2)
	}

	bool newMeasurements = false;
	int pin;
	int interval;
	int samplingRate;
	float PH = -1;
	float voltageCorrection = 5.0 / 1024;
	float temperature = 25;
	unsigned long nextReadTime = 0;
	unsigned long nextSampleTime = 0;

	int sampleValues[PH_NUM_SAMPLES] = {};
	int sampleIndex = -1;
};

#endif