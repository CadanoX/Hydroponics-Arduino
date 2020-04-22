#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>

class Pump
{
public:
  /*
  Typically a pump is connected with 1 pin
  If we use a motor to multiplex multiple pumps, we require 2 pins.
  The motor allows for rotation in two directions using pin states (1,0) and (0,1).
  Since the rotation is irrelevant for the pump, we always trigger the first pin.
  */
  Pump(int pin, int pin2 = -1) : pin(pin),
                                 pin2(pin2)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    if (pin2 != -1)
    {
      pinMode(pin2, OUTPUT);
      digitalWrite(pin2, LOW);
    }
  }

  ~Pump()
  {
    this->stop();
  }

  // Activate the pump
  // If timer (milliseconds) is set, the pump will automatically shut down
  void start(unsigned long timer = 0)
  {
    // Do not start when pump is running
    if (this->state != IDLE)
      return;

    // Activate pump
    digitalWrite(this->pin, HIGH);

    // If time is defined, set it
    if (timer > 0)
    {
      this->state = RUNNING_ON_TIMER;
      this->timer = timer;
    }
    else
      this->state = RUNNING;
  }

  void stop()
  {
    // Do not stop when pump is off
    if (this->state == IDLE)
      return;

    digitalWrite(this->pin, LOW);
    this->state = IDLE;
  }

  // Frequently check if timer ran out
  void check(unsigned long currentTime)
  {
    if (this->state == RUNNING_ON_TIMER)
      if (this->timer < currentTime)
        this->stop();
  }

private:
  enum State
  {
    IDLE,
    RUNNING,
    RUNNING_ON_TIMER
  } state;
  int pin;
  int pin2;
  unsigned long timer = 0;
};

#endif