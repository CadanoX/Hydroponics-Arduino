#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <Arduino.h>
#include <DHT.h>

class DhtSensor
{
public:
  DhtSensor(int rx, int interval = 1000)
  {
    this->interval = interval;
    this->sensor = new DHT(rx, DHT22);
    this->sensor->begin();
  }
  ~DhtSensor()
  {
    delete this->sensor;
  }

  void read()
  {
    this->temperature = this->sensor->readTemperature();
    this->humidty = this->sensor->readHumidity();
    this->newMeasurements = true;
  }

  void check(unsigned long currentTime = 0)
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

  float getHumidty()
  {
    this->newMeasurements = false;
    return this->humidty;
  }

private:
  DHT *sensor;
  bool newMeasurements = false;
  unsigned long nextReadTime = 0;
  int interval = 0;
  float temperature = -999;
  float humidty = -999;
};

#endif