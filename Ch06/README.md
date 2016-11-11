## Synopsis - Steering the Crawler

Create a system that localizes a mobile device(e.g., on a person or vehicle)in an indoor environment. The results should be displayed graphically in real time on a host computer display that is not directly connected to the mobile device. 

### Learning Objectives

Learn the different strategies for indoor localization (positioning); adaptation of these strategies to the XBees or Photons and other available sensors. 

## Installation & Setup & System Overview

![alt text](https://github.com/aparolfe/Group_13_Story/blob/ch06/Ch06/static/systemoverview.png)

## Setup and Demo

### Arduino

Plug an Arduino into your computer.

In the folder Ch05, and open the control.ino sketch in the Arduino IDE and upload the program on the arduino. If you need to debug the program, open up the Serial Monitor from the console fromt he tools menu. 

Next configure the XBees. Configure 
_NOTE_ : Make sure the correct device is selected in the Tools/Ports menu, then upload the sketch to the Arduino. If you are programming the arduino with it's XBee shield then make sure it is in DLINE mode.

### XBee

Configure the XBee by following the steps shown in [XBee setup guide and configuration](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-XBee-Setup). 

#### RSSI from XBee

To obtain the RSSI from the beacons(stack of arduino and a XBee shield) use the following code on the arduino ![arduino code](https://github.com/EC544-BU/EC544_demos/blob/master/demos/rssi/rssiBeacon/rssiBeacon.ino) and the following NodeJS server on the localhost ![NodeJS Server code](https://github.com/EC544-BU/EC544_demos/blob/master/demos/rssi/rssiBeacon/rssiBeacon.ino) 

### Webpage

We have our own webpage that displays the position of the given device tracked

We use a NodeJS server to render a HTML page.

## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan





