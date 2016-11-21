#include <XBee.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

XBee xbee = XBee();
SoftwareSerial xbeeSerial(2, 3);
ZBRxResponse rxResponse = ZBRxResponse();
ZBTxRequest txRequest;

const int BLUE_LED = 6;  // leader
const int RED_LED = 4;  // infected
const int GREEN_LED = 5;  // clear
const int PIN_BUTTON = 8;   // infection button

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers



bool isleader = true;
bool isinfected = false;
int wait_for_Leader_heart_beat = 4000;
int heartbeat_period = 5000;
int infection_period = 2000;
int immunity_period = 3000;
int election_period = 3000;
char myID;

uint8_t message;
const uint8_t IsthereLeader = 0xC1,
              HEARTBEAT = 0xC2,
              CLEAR = 0xC3,
              INFECTION = 0xC4;

uint32_t last_immune = millis();
uint32_t last_heartbeat = millis();
uint32_t last_infected = millis();
uint32_t last_election = millis();



void setup() {

  Serial.begin(57600);
  xbeeSerial.begin(57600);
  xbee.begin(xbeeSerial);

  //for (int i = 0; i < 1; i++) {
    myID = EEPROM.read(0);
 // }
  Serial.println("Read");
  Serial.println(myID);

  pinMode(PIN_BUTTON, INPUT);

  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Make all LED's on to indicate that we are in the setup mode
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  delay(2000);



  sendCommand(0x0000FFFF, (uint8_t*)&  IsthereLeader, 1); // is there any leader in the network

  delay(wait_for_Leader_heart_beat); //wait to hear if there is a heart beat from the leader

  //if ( there is feedback)
  // I'm not a leader
  //
  // else
  //
  //  go to election()

}

//============ This Function To SET LED's =====================

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

//========================== Send API Masseges====================

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

//=================== Read from API ==========================


//============================================================

//============= Election Method================
void elect(void)
{
  last_election = millis();
  sendCommand(0x0000FFFF, (uint8_t*) & myID, 1);
  Serial.println("election");
  while (millis() < last_election + election_period) {
    if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rxResponse);
      message = rxResponse.getData(0);
      Serial.println(message);
      Serial.println(myID);
      if(message >  (int)myID){
        Serial.println("compare");
        isleader = false;
        set_leds();
        break;
      }
    }

  }
  set_leds();
}
//=============================================

void infect(void) {
  if (millis() > last_immune + immunity_period) {
    isinfected = true;
    set_leds();
    sendCommand(0x0000FFFF, (uint8_t*) & INFECTION, 1);
    last_infected = millis();
  }
}
void disinfect(void) {
  isinfected = false;
  set_leds();
  last_immune = millis();
  sendCommand(0x0000FFFF, (uint8_t*) & CLEAR, 1);
}

void loop() {

  if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
    xbee.getResponse().getZBRxResponse(rxResponse);
    message = rxResponse.getData(0);

    Serial.println(message);
    switch (message) {
      case HEARTBEAT:
        { Serial.println("HEARTBEAT");
          if (isleader) elect();
          else last_heartbeat = millis();
          break;
        }
      case INFECTION: if (!isleader) infect(); break;
      case CLEAR: if (!isleader) disinfect(); break;
      default: elect(); break;
    }
  }
  

  if (isleader && (millis() > last_heartbeat + heartbeat_period)) {
    sendCommand(0x0000FFFF, (uint8_t*) & HEARTBEAT, 1);
    last_heartbeat = millis();
  }

  if (isinfected && (millis() > last_infected + infection_period)) {
    sendCommand(0x0000FFFF, (uint8_t*) & INFECTION, 1);
    last_infected = millis();
  }

  //debouncing
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

      // only toggle the LED if the new button state is HIGH
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





