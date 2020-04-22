#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

class Relay
{
public:
  Relay(int pin) : pin(pin)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); // HIGH means relay is switched off
  }

  ~Relay()
  {
    this->stop();
  }

  void start()
  {
    if (!this->isOn)
    {
      digitalWrite(this->pin, LOW);
      this->isOn = true;
    }
  }

  void stop()
  {
    if (this->isOn)
    {
      digitalWrite(this->pin, HIGH);
      this->isOn = false;
    }
  }

private:
  int pin;
  bool isOn = false;
};

#endif