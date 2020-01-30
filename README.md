# Arduino code for O.L.D.

Find the server code under https://github.com/CadanoX/Hydroponics

ARDUINO over USB is at /dev/ttyACM0 or /dev/ttyACM1

# Setup arduino-cli for command line Arduino Sketch
- 64bit (ubuntu bash):
`wget https://downloads.arduino.cc/arduino-cli/arduino-cli-latest-linux64.tar.bz2`
- ARM (rasppi):
`wget https://downloads.arduino.cc/arduino-cli/arduino-cli-latest-linuxarm.tar.bz2`
```sh
sudo tar -xvf arduino-cli-latest-linux64.tar.bz2
rm arduino-cli-latest-linux64.tar.bz2
sudo mv arduino-cli-0.3.3-alpha.preview-linux64 /usr/local/bin/arduino-cli
arduino-cli core update-index
arduino-cli core install arduino:avr
```

The sketch must be located in /home/pi/Arduino/olduino and the .ino file must have the same name as the folder (e.g. olduino.ino)

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328 /home/pi/Arduino/olduino
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328 /home/pi/Arduino/olduino
```

# Install libraries
```sh
arduino-cli lib search onewire
arduino-cli lib install "OneWire" "DallasTemperature"
```

picocom -b 115200 -r -l /dev/ttyUSB0

# Alternative: Setup http://inotool.org/ for command line Arduino Sketch
```sh
sudo apt-get install arduino
sudo apt-get install picocom
git clone git://github.com/amperka/ino.git
cd ino
sudo make install
cd ..
sudo rm -rf ino
pip install congifobj jinja2 glob2 serial
```

## Upload the project to Arduino
- using a mega2560
```
ino build
ino upload
ino serial
ctrl A + ctrl X
```

- using a nano328
```
ino build -m mega2560
ino upload -m mega2560 -p /dev/ttyACM1
ino serial -p /dev/ttyACM1
```


## Enable the Serial Port on Arduino
`sudo chmod 666 /dev/ttyACM0`

if working on Windows Linux Subsystem, COM5 is mapped to ttyS5:

`sudo chmod 666 /dev/ttyS5`

# Possibly necessary changes to make it work
Configuration for the Arduino serial connection:

```sh
sudo stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
sudo usermod -a -G dialout www-data
```

## avrdude: stk500_recv(): programmer is not responding
 make sure that no device is communicating with the Arduino, so stop the server on the Raspberry before uploading a sketch to Arduino
https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/windows---quick-and-easy

# Sensor libraries
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