## Synopsis - Steering the Crawler

Create a system that drives your vehicle through an indoor course without operator (human) control. The system should leverage control strategies for driving the collision avoidance (from challenge 4) and indoor positioning (from challenge 6).  Your solution should also provide a real time data stream (challenge 3) including position information that should be displayed on a remote computer console(challenge 2)illustrating position information of your vehicle. Your systemshould also allow a human to take control remotely (challenge 3). 

## Installation & Setup

Learning objectives:
1. learn how to achieve actuation with networked devices
2. servo control, use rangefinder devices, learn limitations of networked control loops, sensors,and actuators.

##System Overview

![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch05/static/systemoverview.png)

## Setup and Demo

### Arduino

Plug an Arduino into your computer.

In the folder Ch05, and open the control.ino sketch in the Arduino IDE and upload the program on the arduino. If you need to debug the program, open up the Serial Monitor from the console fromt he tools menu. 

Next configure the XBees. Configure 
_NOTE_ : Make sure the correct device is selected in the Tools/Ports menu, then upload the sketch to the Arduino. If you are programming the arduino with it's XBee shield then make sure it is in DLINE mode.

### XBee

Configure the XBee by following the steps shown in [XBee setup guide and configuration](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-XBee-Setup). 

### Webpage

We have our own webpage that allows start and stop of the Crawler

We use a NodeJS server to render a HTML page.

Our crawler

![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch05/static/IMG_20161101_112737.jpg)

LIDAR sensor mounting 
![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch05/static/IMG_20161101_112755.jpg)

IR sensor mounting 
![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch05/static/IMG_20161101_112807.jpg)

## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan





