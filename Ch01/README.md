## Synopsis

A system that will **continuously measure (0.5 Hz, every seconds)** temperature in five locations in the room, compute the **instantaneous average temperature** and report these values to a central station in **calibrated engineering units**

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

### XBee Serial Monitor using Node.js

If all your packages for Node.js are installed as shown in [Node.js setup guide](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-Node.js-Setup),you should be able to run the Xbee using Nodejs.

Connect the XBee coordinator to your computer and run the command

```
node listPorts.js
```

Then identify the XBee device and it's port and run the command

```
node Nest.js <Device>

```

you should be able to see the output on [http://localhost:3000](http://localhost:3000)
## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan

