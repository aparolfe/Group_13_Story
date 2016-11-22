## Synopsis - Infectious Swarm

Create a swarm with the ArduinoXBee or Photons. The swarm will react to an “infection” triggered by a button push

### Learning Objectives

Swarming, multihop, distributed algorithms, leader election. 

##Design Decisions

1. XBees Related items                 
	AT Mode vs. API Mode
	Network without Coordinator
2. Election Algorithm 
	All motes broadcast IDs (from Arduino EEPROM)
	Mote with highest ID is leader
	All motes are followers on startup to minimize the number of elections

## Setup and Demo

### Arduino

Plug an Arduino into your computer.

In the folder Ch07, and open the swarm_v2/swarm_v2.ino sketch in the Arduino IDE and upload the program on the arduino. If you need to debug the program, open up the Serial Monitor from the console fromt he tools menu. 

Next configure the XBees. Configure 
_NOTE_ : Make sure the correct device is selected in the Tools/Ports menu, then upload the sketch to the Arduino. If you are programming the arduino with it's XBee shield then make sure it is in DLINE mode.

### XBee

Configure the XBee by following the steps shown in [XBee setup guide and configuration](https://github.com/EC544-BU/EC544_demos/wiki/Guide:-XBee-Setup). 

We connfigured all our XBees in API mode and all of them as routers. 

###Demonstration

[![IMAGE ALT TEXT HERE](http://i3.ytimg.com/vi/UTZT8w-q2qw/hqdefault.jpg)](https://www.youtube.com/watch?v=UTZT8w-q2qw&feature=youtu.be)

##Algorithm 

###Leader Election
- If HeartBeat(leader alive message) not received: trigger election.
- If leader hears HeartBeat from someone else: trigger election
        - Find device with highest id#
        - Update all device statuses to leader or not-leader


###Non-leader behavior
- Listen for message
    - If infection message received:
        RED led on
        Become infected
        Continuously broadcast infected message 
    - If clear message received:
        GREEN led on
        Return to clear state
        Send clear message
        Immune for 3 seconds
    - If button pressed:
- Become infected

###Leader behavior
- BLUE led on
- Send HeartBeat
- If button pressed:
    Send clear message

## Contributors

1. Aparna Rolfe
2. Prerana Haridoss
3. Mohammed Alabdemohsen
4. Kuangyu (Allen) Yan






