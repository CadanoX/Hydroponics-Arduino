#ifndef DFROBOT_CO2_H
#define DFROBOT_CO2_H

// the DFRobot sensor needs to aggregate measurements to provide correct results
#define NUM_SAMPLES 10
#define VOLTAGE 5.0
#define BITRANGE 1024 // arduino uses 10 bits on analogRead --> 2^10 = 1024
#define VOLTAGE_MIN 0.4
#define VOLTAGE_MAX 2.0
#define VOLTAGE_RANGE (VOLTAGE_MAX - VOLTAGE_MIN)
#define MAX_PPM 5000

#include <Arduino.h>

class DFRobot_CO2
{
public:
  DFRobot_CO2(int pin, int interval = 1000, int samplingRate = 80)
      : pin(pin),
        interval(interval),
        samplingRate(samplingRate)
  {
    this->initSampleValues();
  }

  ~DFRobot_CO2()
  {
  }

  /* Measured values are between 0 and BITRANGE
   * From the value, calculate voltage between 0 and VOLTAGE
   * From the voltage, calculate ppm between 0 and 5000
   * If returned ppm is negative, the sensor is still warming up
   */
  void read()
  {
    // Calculate voltage from the average sample value
    float voltage = avgSampleValues() / (BITRANGE - 1) * VOLTAGE;
    // Bring value between 0 and 1
    float norm = (voltage - VOLTAGE_MIN) / VOLTAGE_RANGE;
    // Calculate ppm
    this->CO2 = norm * MAX_PPM;
    this->newMeasurements = true;
  }

  void write(char *command) {}

  // Frequently check if a new measure should be taken
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

  float getCO2()
  {
    this->newMeasurements = false;
    return this->CO2;
  }

private:
  // Read sensor once
  void sample()
  {
    sampleValues[sampleIndex++] = analogRead(this->pin);
    // Prevent array from overflows
    if (sampleIndex == NUM_SAMPLES)
      sampleIndex = 0;
  }

  // Read measurements until sample array is full
  void initSampleValues()
  {
    sampleIndex = -1;
    for (int i = 0; i < NUM_SAMPLES; i++)
      sample();
  }

  // average all sampled values except the lowest and highest
  double avgSampleValues()
  {
    int min, max;
    long sum = 0;

    // initialize min and max values with the first 2 values
    if (sampleValues[0] < sampleValues[1])
    {
      min = sampleValues[0];
      max = sampleValues[1];
    }
    else
    {
      min = sampleValues[1];
      max = sampleValues[0];
    }

    // only add the value to the sum, if it is neither the min, nor max value
    for (int i = 2; i < NUM_SAMPLES; i++)
    {
      if (sampleValues[i] < min)
      {
        sum += min;
        min = sampleValues[i];
      }
      else if (sampleValues[i] > max)
      {
        sum += max;
        max = sampleValues[i];
      }
      else
        sum += sampleValues[i];
    }

    return (double)sum / (NUM_SAMPLES - 2);
  }

  bool newMeasurements = false;
  int pin;
  int interval;
  int samplingRate;
  float CO2 = -1;
  float temperature = 25;
  unsigned long nextReadTime = 0;
  unsigned long nextSampleTime = 0;

  int sampleValues[NUM_SAMPLES] = {};
  int sampleIndex = -1;
};

#endif