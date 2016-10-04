## Synopsis

Create a system that will save data collected from a network of sensors and will allow visualization of data in both historical and real time views. Data should be hosted two ways: (a) on the localhost (laptop) and (b) on the pi.

## Installation

All the installations are as shown in the [setup guide](https://piazza-resources.s3.amazonaws.com/iqwd42aencm43a/isrn6dbyn8wh/Software_Installs.pdf?AWSAccessKeyId=AKIAIEDNRLJ4AZKBW6HA&Expires=1474313472&Signature=6J3Yzfwdw%2FHmT%2ByzIzeAZOX6x%2BQ%3D) or use the [setup guide on github](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-Getting-Started).

## Setup and Demo


### Arduino

Plug an Arduino into your computer.

In the folder Ch01, and open the Router_code.ino sketch in the Arduino IDE and upload the program on the arduino. If you need to debug the program, open up the Serial Monitor fromt he console fromt he tools menu. 

Next configure the XBees. Configure 
_NOTE_ : Make sure the correct device is selected in the Tools/Ports menu, then upload the sketch to the Arduino. If you are programming the arduino with it's XBee shield then make sure it is in DLINE mode.

### XBee

Configure the XBee by following the steps shown in [XBee setup guide and configuration](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-XBee-Setup). 

We configure the routers(AT mode) and the co-ordinator(API mode) using this guide.

###Setup Rpi

1. Install OS on the RPi from the SD card. 
2. Connect to Rpi using the cable and then change the setting to setup wireless communication using the WiFi Dongle3. Edit the /etc/wpa_supplicant/wpa_supplicant.conf from 

```
network={
    ssid="The_ESSID_from_earlier"
    psk="Your_wifi_password"
}
```
to 

```
network={
    ssid=<Router SSID>
    psk=<Router password>
}
```

4. Install Node.js and npm
3. Install VNC remote desktop

_NOTE_: To install serial port use this command
```
npm install serialport -unsafe -perm
```
To install SQLite3
```
npm install sqlite3 -unsafe -perm
```

### Node.js on RPi

Git clone the repository from [GitHub repository](https://github.com/aparolfe/Group_13_Story.git) using:

```
git clone https://github.com/aparolfe/Group_13_Story.git
```
Navigate to the folder Ch02 for all the files relating to Challenge 2.

If all your packages for Node.js are installed as shown in [Node.js setup guide](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-Node.js-Setup),you should be able to run the Xbee using Nodejs.

Connect the XBee coordinator to your RPi and run the command on the RPi(localhost)

```
node listPorts.js
```

Then identify the XBee device and it's port and run the command RPi(localhost)

```
node Nest.js <Device>

```

you should be able to see the output on [http://localhost:3000](http://localhost:3000)

OR on the [http://<IP of the RPi>:3000](http://localhost:3000) if connected to the same network


## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan


