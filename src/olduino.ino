#include <SoftwareSerial.h>
#include "functions.h"
#include "DS18B20.h"
#include "EcSensor.h"
#include "DFRobot_PH.h"

// EC on Hardware Serial 2 (pin 17,16)
#define pinPhSensor A0
#define pinDS18B20 11

SoftwareSerial ecSerial(2, 3);

DS18B20 tempWaterSensor(pinDS18B20);
EcSensor ecSensor(ecSerial);
DFRobot_PH phSensor(pinPhSensor);

static char serialReadBuffer[80];
int serialReadPos = 0;

static unsigned long currentTime = 0;

void executeCommand(char* deviceType, int deviceNr, char* command)
{
	if (command == NULL)
		return;
	
	if (strcmp(deviceType, "sensor") == 0)
	{
		switch(deviceNr)
		{
			/*case 0: // pH sensor
				phSensor.write(command);
			break;*/
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
}

void checkSerialInput()
{
    if (readline(Serial.read(), serialReadBuffer, 80, serialReadPos) > 0)
	{
		// parse the string in the buffer to the command parts (receiver-nr command)
		char* deviceType = strtok(serialReadBuffer, "-");
		char* deviceNr = strtok(NULL, " ");
		char* command = strtok(NULL, " ");

		executeCommand(deviceType, atoi(deviceNr), command);

		Serial.print(deviceType);
		Serial.print("-");
		Serial.print(deviceNr);
		Serial.print(" ");
		Serial.println(command);
	}
}

void checkSensorInput()
{
	currentTime = millis();
	ecSensor.read();
	phSensor.check(currentTime);
	tempWaterSensor.check(currentTime);
}

void printSensorValues()
{
	// if no sensor has new measurements, don't send any message
	if (ecSensor.hasNewMeasurements() ||
		phSensor.hasNewMeasurements() ||
		tempWaterSensor.hasNewMeasurements())
	{
		// send measurements to raspberry in JSON format
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

		if(phSensor.hasNewMeasurements())
		{
			Serial.print("\"PH\": ");
			dtostrf(phSensor.getPh(), 8, 3, floatHelp);
			Serial.print(floatHelp);
			Serial.print(",");
		}

		if(ecSensor.hasNewMeasurements())
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

		Serial.print("\"Debug\": 0"); // neccessary in order not to end on a comma
		Serial.println('}');

		Serial.flush(); // wait until the string was sent
	}
}

void setup()
{
	Serial.begin(9600);
	ecSerial.begin(9600);
	while (!Serial) {;} // wait for serial port to connect. Needed for native USB port only
}

void loop()
{
	checkSerialInput();
	checkSensorInput();
	printSensorValues();
}