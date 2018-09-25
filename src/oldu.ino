#include "Pump.cpp"

Pump pumpTEstA(1);

static char serial1ReadBuffer[80];
int serialReadPos = 0;

int readline(int readch, char *buffer, int len, int& pos)
{
	int rpos;

	if (readch > 0)
	{
		switch (readch)
		{
			case '\n': // Ignore new-lines
				break;
			case '\r': // Return on CR
				rpos = pos;
				pos = 0;  // Reset position index ready for next time
				return rpos;
			default:
				if (pos < len-1) {
					buffer[pos++] = readch;
					buffer[pos] = 0;
				}
		}
	}
	// No end of line has been found, so return -1.
	return -1;
}

void setup()
{
	Serial.begin(9600);
}

void loop()
{
    if (readline(Serial.read(), serial1ReadBuffer, 80, serialReadPos) > 0)
	{
		// parse the string in the buffer to the command parts (receiver-nr command)
		char* deviceType = strtok(serial1ReadBuffer, "-");
		char* deviceNr = strtok(NULL, " ");
		char* command = strtok(NULL, " ");

		Serial.print(deviceType);
		Serial.print("-");
		Serial.print(deviceNr);
		Serial.print(" ");
		Serial.println(command);
	}
}