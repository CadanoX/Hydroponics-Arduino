#include "functions.h"
#include "DhtSensor.h"
#include "DS18B20.h"
#include "EcSensor.h"
#include "CO2Sensor.h"
#include "PhSensor.h"
#include "Pump.h"
#include "Relay.h"

// O2 on Hardware Serial 2 (pin 19,18)
// EC on Hardware Serial 2 (pin 17,16)
// PH on Hardware Serial 3 (pin 15,14)
#define motor1EnA 2
#define pin1pump1 3 // pump pH increase
#define pin2pump1 4 // pump pH increase
#define pin1pump2 5 // pump pH decrease
#define pin2pump2 6 // pump pH decrease
#define motor1EnB 7
#define motor2EnA 8
#define pin1pump3 9 // pump EC increase
#define pin2pump3 10 // pump EC increase
#define pin1pump4 11 // pump O2 increase
#define pin2pump4 12 // pump O2 increase
#define motor2EnB 13
#define pinDht 32
#define pinDS18B20 41
#define pinRelay1 47
#define pinRelay2 51
#define pinRelay3 46
#define pinRelay4 50

DhtSensor dhtSensor(pinDht);
DS18B20 tempWaterSensor(pinDS18B20);
EcSensor ecSensor;
PhSensor phSensor;
CO2Sensor co2Sensor;
Pump pumpPhIncr(pin1pump1, pin2pump1);
Pump pumpPhDecr(pin1pump2, pin2pump2);
Pump pumpEcIncr(pin1pump3, pin2pump3);
Pump pumpO2Incr(pin1pump4, pin2pump4);
Relay relay1(pinRelay1);
Relay relay2(pinRelay2);
Relay relay3(pinRelay3);
Relay relay4(pinRelay4);

static char serial1ReadBuffer[80];
int serial1ReadPos = 0;

static unsigned long currentTime = 0;

void executeCommand(char* deviceType, int deviceNr, char* command)
{
	if (command == NULL)
		return;
	
	if (strcmp(deviceType, "sensor") == 0)
	{
		switch(deviceNr)
		{
			case 0: // pH sensor
				phSensor.write(command);
			break;
			case 1: // EC sensor
				ecSensor.write(command);
			break;
			case 3: // O2 sensor
				co2Sensor.write(command);
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
		char* com = strtok(command, ","); // read command
		char* arg = strtok(NULL, " "); // read argument
		int comNum = atoi(com);
		int millisec = atoi(arg);

		Pump* pump;
		if (deviceNr == 0) pump = &pumpPhIncr;
		else if (deviceNr == 1) pump = &pumpPhDecr;
		else if (deviceNr == 2) pump = &pumpEcIncr;
		else if (deviceNr == 3) pump = &pumpO2Incr;
		else {
			Serial.print("Pump ");
			Serial.print(deviceNr);
			Serial.println(" is not defined.");
			return;
		}

		if (comNum == 0)
			pump->stop();
		else if (comNum == 1) // turn pump on
		{
			if(millisec) {
				pump->start(currentTime + millisec);
			}
			else
				pump->start();
		}
	}
	else if (strcmp(deviceType, "relay") == 0)
	{
		char* com = strtok(command, ","); // read command
		int comNum = atoi(com);

		Relay* relay;
		if (deviceNr == 0) relay = &relay1;
		else if (deviceNr == 1) relay = &relay2;
		else if (deviceNr == 2) relay = &relay3;
		else if (deviceNr == 3) relay = &relay4;
		else {
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

void setup()
{
	Serial.begin(9600);

	// these pins are needed, because pumps are run over a motor
	// TODO: write motor class instead of pumps
	pinMode(motor1EnA, OUTPUT);
	pinMode(motor1EnB, OUTPUT);
	pinMode(motor2EnA, OUTPUT);
	pinMode(motor2EnB, OUTPUT);
	analogWrite(motor1EnA, 200);
	analogWrite(motor1EnB, 200);
	analogWrite(motor2EnA, 200);
	analogWrite(motor2EnB, 200);
}

void loop()
{
	currentTime = millis();

    if (readline(Serial.read(), serial1ReadBuffer, 80, serial1ReadPos) > 0)
	{
		// parse the string in the buffer to the command parts (receiver-nr command)
		char* deviceType = strtok(serial1ReadBuffer, "-");
		char* deviceNr = strtok(NULL, " ");
		char* command = strtok(NULL, " ");

		executeCommand(deviceType, atoi(deviceNr), command);

		Serial.print(deviceType);
		Serial.print("-");
		Serial.print(deviceNr);
		Serial.print(" ");
		Serial.println(command);
	}

	// check if pumps need to be shut off
	pumpPhIncr.check(currentTime);
	pumpPhDecr.check(currentTime);
	pumpEcIncr.check(currentTime);
	pumpO2Incr.check(currentTime);

	// read sensor values
	ecSensor.read();
	phSensor.read();
	co2Sensor.read();
	dhtSensor.check(currentTime);
	tempWaterSensor.check(currentTime);

	// if no sensor has new measurements, don't send any message
	if (ecSensor.hasNewMeasurements() ||
		phSensor.hasNewMeasurements() ||
		co2Sensor.hasNewMeasurements() ||
		dhtSensor.hasNewMeasurements() ||
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

		if(co2Sensor.hasNewMeasurements())
		{
			Serial.print("\"O2\": ");
			dtostrf(co2Sensor.getCO2(), 8, 3, floatHelp);
			Serial.print(floatHelp);
			Serial.print(",");
		}

		Serial.print("\"Debug\": 0"); // neccessary in order not to end on a comma
		Serial.println('}');

		Serial.flush(); // wait until the string was sent
	}
}