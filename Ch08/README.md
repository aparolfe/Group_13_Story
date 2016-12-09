## Synopsis - Steering the Crawler

Create a system that drives your vehicle through an indoor course without operator (human) control. The system should leverage control strategies for driving the collision avoidance (from ![challenge 4](https://github.com/aparolfe/Group_13_Story/tree/master/Ch04)) and indoor positioning (from ![challenge 6](https://github.com/aparolfe/Group_13_Story/tree/master/Ch06)).  Your solution should also provide a real time data stream (![challenge 3](https://github.com/aparolfe/Group_13_Story/tree/master/Ch03)) including position information that should be displayed on a remote computer console(![challenge 2](https://github.com/aparolfe/Group_13_Story/tree/master/Ch02)illustrating position information of your vehicle. Your systemshould also allow a human to take control remotely (![challenge 3](https://github.com/aparolfe/Group_13_Story/tree/master/Ch03)). 

Integrating the following solutions:
- Autonomous driving and collision avoidance using provided sensors, speed, control and servo steering.
- Indoor location sensing and reporting to a base station to log location information at the base station
- Real-time visualization of position based on logged information, displayed at a remote terminal.
- Remote control from a disparate IP network to permit course corrections and safety features
and to take over control of vehicle
- Firstperson video streaming from the crawler to the remote site where the crawler is being controlled

##Learning objectives

1. learn how to achieve actuation with networked devices
2. servo control, use rangefinder devices, learn limitations of networked control loops, sensors,and actuators.

##Design Decisions

###Hardware
- Sharp PIR sensor on front and back of crawler. Used to sense danger of collision
- One LIDAR range sensor on each side of crawler. Sensors mounted towards the front of the chassis. Crawler doesn’t rely on a continuous wall on one side.
- Arduino for driving the crawler. Leveraging and building on ![Challenge 5 solution](https://github.com/aparolfe/Group_13_Story/tree/master/Ch05)
- RPi as functionality hub. Leveraging Challenge 6 solution (Localization). USB port for video. Serial comm with Arduino

###Software-Control Stratergy
- PID for straight corridors
- Turn Strategies: 
- If distance from wall > 1000: stop, reverse and turn 
- If wall detected, then: stop, reverse and turn 
- Avoids elevator cove
- Left sensor reading indicates the cove

###Software - Localisation/UI
- Beacon placement: mid-corridors
    - Reduces max distance of crawler from farthest beacon
    - RSSI signal more reliable at closer distance
- Node.js server on port 3000 of the Rpi 
    - Handles localization
    - Relays remote commands to Arduino
- Video stream server on port 8080 of the Rpi
- Website combines information from both Rpi services
    - Manual mode driving using video-game keypresses
- W-forward, D-right, X-reverse, S-left

##System Overview

![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch08/static/systemoverview.png)

## Setup and Demo

### Arduino

1. Plug an Arduino into your computer.
2. In the folder Ch08, and open the control_pid folder and load the sketch in the Arduino IDE and upload the program on the arduino. If you need to debug the program, open up the Serial Monitor from the console from the tools menu. 
_NOTE_ : Make sure the correct device is selected in the Tools/Ports menu, then upload the sketch to the Arduino. If you are programming the arduino with it's XBee shield then make sure it is in DLINE mode.

### XBee

Configure the XBee by following the steps shown in [XBee setup guide and configuration](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-XBee-Setup). 

### Webpage

![alt_text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch08/static/screenshot.png)

We have our own webpage that displays the following components:
- Live Video Stream
- Localisation
- Manual control buttons
- Start/Stop buttons 
- Indicator of manual and auto mode
 
We use a NodeJS server to render a HTML page. 

Our crawler:

![alt text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch05/static/IMG_20161101_112737.jpg)

![alt_text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch08/static/crawler2.jpg)

![alt_text](https://github.com/aparolfe/Group_13_Story/blob/master/Ch08/static/crawler3.jpg)

## Contributors
1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan





