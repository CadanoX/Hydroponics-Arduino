#include <SoftwareSerial.h>
#include "src/functions.h"
#include "src/DhtSensor.h"
#include "src/DS18B20.h"
#include "src/EcSensor.h"
#include "src/DFRobot_PH.h"
#include "src/Pump.h"
#include "src/Relay.h"

// running multiple pumps via a motor requires control pins
#define motor1EnA 2
#define motor1EnB 7
#define motor2EnA 8
#define motor2EnB 13

// Set hardware pins for connected devices
DhtSensor dhtSensor(32);
DS18B20 tempWaterSensor(41);
DFRobot_PH phSensor(A0);
// EC sensor uses software serial
SoftwareSerial ecSerial(15, 16);
EcSensor ecSensor(ecSerial);
Pump pumpPhIncr(3, 4);
Pump pumpPhDecr(5, 6);
Pump pumpEcIncr(9, 10);
Pump pumpO2Incr(11, 12);
Relay relay0(47);
Relay relay1(51);
Relay relay2(46);
Relay relay3(50);

static char serialReadBuffer[80];
int serialReadPos = 0;
static unsigned long currentTime = 0;

/*
Send commands to connected devices

deviceType: "sensor"
deviceNr:
0 = pH sensor
1 = EC sensor
commands depend on the sensor and are listed in the sensor manuals

deviceType: "pump"
deviceNr:
0 = pH+ pump
1 = pH- pump
2 = EC+ pump
3 = O2+ pump
command:
1 = turns the pump on
1,2000 = turns the pump on for 2 seconds
0 = turns pump off

"pump", "relay"
*/
void sendCommand(char *deviceType, int deviceNr, char *command)
{
  if (command == NULL)
    return;

  if (strcmp(deviceType, "sensor") == 0)
  {
    switch (deviceNr)
    {
    case 0: // pH sensor
      phSensor.write(command);
      break;
    case 1: // EC sensor
      ecSensor.write(command);
      break;
    default:
      Serial.print("Sensor ");
      Serial.print(deviceNr);
      Serial.println(" is not defined.");
      break;
    }
  }
  else if (strcmp(deviceType, "pump") == 0)
  {
    // split pump command in 2 comma separated components
    char *com = strtok(command, ","); // read command
    char *arg = strtok(NULL, " ");    // read argument
    int comNum = atoi(com);
    int millisec = atoi(arg);

    Pump *pump;
    if (deviceNr == 0)
      pump = &pumpPhIncr;
    else if (deviceNr == 1)
      pump = &pumpPhDecr;
    else if (deviceNr == 2)
      pump = &pumpEcIncr;
    else if (deviceNr == 3)
      pump = &pumpO2Incr;
    else
    {
      Serial.print("Pump ");
      Serial.print(deviceNr);
      Serial.println(" is not defined.");
      return;
    }

    if (comNum == 0)
      pump->stop();
    else if (comNum == 1) // turn pump on
    {
      if (millisec)
        pump->start(millisec);
      else
        pump->start();
    }
  }
  else if (strcmp(deviceType, "relay") == 0)
  {
    char *com = strtok(command, ","); // read command
    int comNum = atoi(com);

    Relay *relay;
    if (deviceNr == 0)
      relay = &relay0;
    else if (deviceNr == 1)
      relay = &relay1;
    else if (deviceNr == 2)
      relay = &relay2;
    else if (deviceNr == 3)
      relay = &relay3;
    else
    {
      Serial.print("Relay ");
      Serial.print(deviceNr);
      Serial.println(" is not defined.");
      return;
    }

    if (comNum == 0)
      relay->stop();
    else if (comNum == 1)
      relay->start();
  }
}

// Get instructions from server
void checkSerialInput()
{
  // Check if the server sent new instructions
  if (readline(Serial.read(), serialReadBuffer, 80, serialReadPos) > 0)
  {
    // Parse the string in the buffer to the command parts (receiver-nr command)
    char *deviceType = strtok(serialReadBuffer, "-");
    char *deviceNr = strtok(NULL, " ");
    char *command = strtok(NULL, " ");

    sendCommand(deviceType, atoi(deviceNr), command);

    Serial.print(deviceType);
    Serial.print("-");
    Serial.print(deviceNr);
    Serial.print(" ");
    Serial.println(command);
  }
}

// Get sensor feedback
void checkDevices()
{
  currentTime = millis();
  ecSensor.read();
  phSensor.check(currentTime);
  tempWaterSensor.check(currentTime);
  dhtSensor.check(currentTime);
  pumpPhIncr.check(currentTime);
  pumpPhDecr.check(currentTime);
  pumpEcIncr.check(currentTime);
  pumpO2Incr.check(currentTime);
}

// Send measurements to server in JSON format
void printSensorValues()
{
  // If no sensor has new measurements, don't send any message
  if (ecSensor.hasNewMeasurements() ||
      phSensor.hasNewMeasurements() ||
      dhtSensor.hasNewMeasurements() ||
      tempWaterSensor.hasNewMeasurements())
  {
    // Convert float values to string
    char floatHelp[10];

    Serial.print('{');

    if (tempWaterSensor.hasNewMeasurements())
    {
      float temp = tempWaterSensor.getTemperature();
      if (!isnan(temp))
      {
        Serial.print("\"WaterTemp\": ");
        dtostrf(temp, 8, 3, floatHelp);
        Serial.print(floatHelp);
        Serial.print(",");
      }
    }

    if (dhtSensor.hasNewMeasurements())
    {
      Serial.print("\"Humidity\": ");
      dtostrf(dhtSensor.getHumidty(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");

      Serial.print("\"Temp\": ");
      dtostrf(dhtSensor.getTemperature(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");
    }

    if (phSensor.hasNewMeasurements())
    {
      Serial.print("\"PH\": ");
      dtostrf(phSensor.getPh(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");
    }

    if (ecSensor.hasNewMeasurements())
    {
      Serial.print("\"EC\": ");
      dtostrf(ecSensor.getEc(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");

      Serial.print("\"TDS\": ");
      dtostrf(ecSensor.getTds(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");

      Serial.print("\"SAL\": ");
      dtostrf(ecSensor.getSal(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");

      Serial.print("\"GRAV\": ");
      dtostrf(ecSensor.getGrav(), 8, 3, floatHelp);
      Serial.print(floatHelp);
      Serial.print(",");
    }

    // Do not end JSON with a comma
    Serial.print("\"Debug\": 0");
    Serial.println('}');

    // Wait until the string was sent
    Serial.flush();
  }
}

void setup()
{
  Serial.begin(9600);
  ecSerial.begin(9600);
  // Other serials are initiated in their class construction

  // Initialize motor control pins
  pinMode(motor1EnA, OUTPUT);
  pinMode(motor1EnB, OUTPUT);
  pinMode(motor2EnA, OUTPUT);
  pinMode(motor2EnB, OUTPUT);
  analogWrite(motor1EnA, 200);
  analogWrite(motor1EnB, 200);
  analogWrite(motor2EnA, 200);
  analogWrite(motor2EnB, 200);

  // Wait for serial port to connect. Needed for native USB port only
  while (!Serial)
  {
    ;
  }
}

void loop()
{
  checkSerialInput();
  checkDevices();
  printSensorValues();
}