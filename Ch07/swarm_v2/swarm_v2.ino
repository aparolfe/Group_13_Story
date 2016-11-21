#include <XBee.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

XBee xbee = XBee();
SoftwareSerial xbeeSerial(2, 3);
ZBRxResponse rxResponse = ZBRxResponse();
ZBTxRequest txRequest;

const int BLUE_LED = 6;   // leader
const int RED_LED = 4;    // infected
const int GREEN_LED = 5;  // clear
const int PIN_BUTTON = 8; // infection button

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

bool isleader = false;
bool isinfected = false;

int heartbeat_timeout = 9999;    // time since last heartbeat before elect trigger
int immunity_timeout = 3000;     // time of immunity
int election_timeout = 3000;     // time to wait before deciding self is leader

int heartbeat_period = 5000;    // time between sent heartbeats
int infection_period = 2000;    // time between sent infection messages

char myID;

uint8_t message;
const uint8_t HEARTBEAT = 0xC1,
              CLEAR = 0xC2,
              INFECTION = 0xC3;

uint32_t last_immune = millis();
uint32_t last_heartbeat = millis();
uint32_t last_infected = millis();
uint32_t last_election = millis();

void setup() {

  Serial.begin(57600);
  xbeeSerial.begin(57600);
  xbee.begin(xbeeSerial);
  myID = EEPROM.read(0);
  Serial.println("Read");
  Serial.println(myID);

  pinMode(PIN_BUTTON, INPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  /*  // Make all LED's on to indicate that we are in the setup mode
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH); */
  set_leds();
}

//============ SET LED's =====================

void set_leds() {
  if (isleader == true)   {
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
  }
  else if (isleader == false && isinfected == true) {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }

  else if (isleader == false && isinfected == false)  {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }
}

//========================== Send API Messages====================

void sendCommand(uint32_t destination_Addr, uint8_t* payload, uint8_t length) {

  //if (payload[0] == MSG_DISCOVERY || payload[0] == MSG_VICTORY) {
  txRequest = ZBTxRequest(XBeeAddress64(0x00000000, 0x0000FFFF), payload, length);
  //Serial.println("SENDING...");
  xbee.send(txRequest);
  /* } else {
     if (destination_Addr == 0x0000FFFF) {
       for (int i = 0; i < numDevices; i++) {
         txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, listAddress64[i]), payload, length);
         xbee.send(txRequest);
       }
     } else {
       txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, destination_Addr), payload, length);
       xbee.send(txRequest);
     }
    } */
}

//============= Election Method ================

void elect(void)
{
  last_election = millis();
  sendCommand(0x0000FFFF, (uint8_t*) & myID, 1);
  Serial.println("election");
  while (millis() < last_election + election_timeout) {
    if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rxResponse);
      message = rxResponse.getData(0);
      Serial.println(message);
      Serial.println(myID);
      if (message >  (int)myID) {
        Serial.println("compare");
        isleader = false;
        set_leds();
        delay(election_timeout);
        return;
      }
    }
  }
  isleader = true;
  sendCommand(0x0000FFFF, (uint8_t*) & HEARTBEAT, 1);
  last_heartbeat = millis();
  set_leds();
}

//============= Infection Method ================

void infect(void) {
  if (millis() > last_immune + immunity_timeout) {
    isinfected = true;
    set_leds();
    sendCommand(0x0000FFFF, (uint8_t*) & INFECTION, 1);
    last_infected = millis();
  }
}

//============= Clear Method ================

void disinfect(void) {
  isinfected = false;
  set_leds();
  last_immune = millis();
  sendCommand(0x0000FFFF, (uint8_t*) & CLEAR, 1);
}

//============= Main Loop ================

void loop() {
  //Listen and react to messages
  if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
    xbee.getResponse().getZBRxResponse(rxResponse);
    message = rxResponse.getData(0);
    Serial.println(message);
    switch (message) {
      case HEARTBEAT:
        { if (isleader) { Serial.println("HEARTBEAT ELECTION"); elect(); }
          else last_heartbeat = millis();
          break;
        }
      case INFECTION: if (!isleader && !isinfected) infect(); break;
      case CLEAR: if (!isleader && isinfected) disinfect(); break;
        //default: Serial.println("default"); elect(); break;
    }
  }
  // Trigger election if no heartbeat for too long
  if (millis() > last_heartbeat + heartbeat_timeout) { last_heartbeat = millis(); Serial.println("Timeout ELECTION"); elect(); }

  // Send messages if appropriate
  if (isleader && (millis() > last_heartbeat + heartbeat_period)) {
    sendCommand(0x0000FFFF, (uint8_t*) & HEARTBEAT, 1);
    last_heartbeat = millis();
  }
/*  if (isinfected && (millis() > last_infected + infection_period)) {
    sendCommand(0x0000FFFF, (uint8_t*) & INFECTION, 1);
    last_infected = millis();
  } */

  // Handle button press
  int reading = digitalRead(PIN_BUTTON);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the LED if the new button state is LOW
      if (buttonState == LOW) {
        if (isleader) disinfect();
        else infect();
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}





