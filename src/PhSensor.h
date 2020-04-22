#ifndef PHSENSOR_H
#define PHSENSOR_H

#include <Arduino.h>

class PhSensor
{
public:
  PhSensor()
  {
    // Initialize serial
    Serial3.begin(9600);
  }

  ~PhSensor()
  {
  }

  // Read sensor measurements
  void read()
  {
    if (readline(Serial3.read(), this->serialReadBuffer, 80, this->readPos) > 0)
    {
      // Filter out communication errors
      if (isdigit(*this->serialReadBuffer))
      {
        this->PH = atof(this->serialReadBuffer);
        this->newMeasurements = true;
      }
    }
  }

  // Send command to sensor
  void write(const char *input)
  {
    Serial3.print(input);
    // Sensor requires <CR> at the end of the string
    Serial3.print('\r');
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