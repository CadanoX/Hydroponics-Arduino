# Arduino code for O.L.D.

Find the server code under https://github.com/CadanoX/Hydroponics\
ARDUINO over USB is at /dev/ttyACM0 or /dev/ttyACM1

# Setup Arduino Sketch on command line

## Option 1: [arduino-cli](https://arduino.github.io/arduino-cli)

Find the correct release for your system here: https://github.com/arduino/arduino-cli/releases\
Replace the name of the file to download in the following commands (e.g. ARMv6 for Raspberry Pi)

```
wget https://github.com/arduino/arduino-cli/releases/download/0.10.0/arduino-cli_0.10.0_Linux_ARMv6.tar.gz
sudo tar -xvf arduino-cli_0.10.0_Linux_ARMv6.tar.gz
rm arduino-cli_0.10.0_Linux_ARMv6.tar.gz
sudo mv arduino-cli /usr/local/bin/arduino-cli
arduino-cli core update-index
arduino-cli core install arduino:avr
```

The sketch .ino must be located in /home/pi/.../projectname and the .ino file must have the same name as the folder (e.g. projectname.ino)

Arduino Mega

```sh
arduino-cli compile --fqbn arduino:avr:mega /home/pi/projectname
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:mega /home/pi/projectname
```

Install libraries

```sh
arduino-cli lib search onewire
arduino-cli lib install "OneWire" "DallasTemperature" "DHT_sensor_library"
```

## Option 2: [inotool](http://inotool.org/)

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

# Enable the Serial Port on Arduino

`sudo chmod 666 /dev/ttyACM0`\
if working on Windows Linux Subsystem, COM5 is mapped to ttyS5:\
`sudo chmod 666 /dev/ttyS5`

# Possibly necessary changes to make it work

Configuration for the Arduino serial connection:

```sh
sudo stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
sudo usermod -a -G dialout www-data
```

# Serial monitor on terminal

Check that no other software is communicating on the channel

```
sudo lsof /dev/ttyS0
```

- Screen

```
sudo apt-get install screen
screen /dev/ttyACM0
```

Exit via Ctrl + A, :, type 'quit', Enter

- Picocom

```
picocom -b 115200 -r -l /dev/ttyUSB0
```

Exit via Ctrl + A, Ctrl + X

# avrdude: stk500_recv(): programmer is not responding

make sure that no device is communicating with the Arduino, so stop the server on the Raspberry before uploading a sketch to Arduino
https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/windows---quick-and-easy

# Sensor libraries

Temperature sensor DHT22
download libraries:

- https://github.com/adafruit/Adafruit_Sensor
- https://github.com/adafruit/DHT-sensor-library

Temperature sensor DS18B20

- uses same temperature library
  /\*
- https://github.com/milesburton/Arduino-Temperature-Control-Library
- http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
  \*/

# When using arduino-cli for SIMD cores, and GLIBC is outdated

```sh
sudo apt-get install dirmngr
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 04EE7237B7D453EC
sudo apt-get update && sudo apt-get upgrade libc6
sudo apt autoremove
```
