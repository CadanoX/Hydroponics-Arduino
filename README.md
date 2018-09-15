**Arduino code for OLD**

ARDUINO over USB is at /dev/ttyACM0 or /dev/ttyACM1

---

## INSTALL http://inotool.org/ FOR COMMAND LINE ARDUINO SKETCH
sudo apt-get install arduino
sudo apt-get install picocom
git clone git://github.com/amperka/ino.git
cd ino
sudo make install
cd ..
sudo rm -rf ino

## CREATE AN ARDUINO PROJECT
mkdir arduinoTest
cd arduinoTest
ino init -t blink
ino build
ino upload
ino serial
ctrl A + ctrl X

-- with mega2560
1. ino build -m mega2560
2. ino upload -m mega2560 -p /dev/ttyACM1
3. ino serial -p /dev/ttyACM1

## WAS THIS IMPORTANT TO MAKE IT WORK??
sudo chmod 777 /dev/ttyACM0
This provides some configuration for the Arduino serial connection:
sudo stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

sudo usermod -a -G dialout www-data

## avrdude: stk500_recv(): programmer is not responding
--> make sure that no device is communicating with the Arduino, so stop the server on the Raspberry before uploading a sketch to Arduino
https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/windows---quick-and-easy

## sensor libraries
Temperature sensor DHT22
download libraries:
- https://github.com/adafruit/Adafruit_Sensor
- https://github.com/adafruit/DHT-sensor-library

Temperature sensor DS18B20
- uses same temperature library
/*
- https://github.com/milesburton/Arduino-Temperature-Control-Library
- http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
*/