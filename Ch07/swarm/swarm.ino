#include <XBee.h>
#include <SoftwareSerial.h>


#define ELECTION_REPLY_WAIT_PERIOD      2000
#define ELECTION_VICTORY_WAIT_PERIOD    5000
#define ELECTION_BETWEEN_WAIT_PERIOD    5000
#define LEADER_HEARTBEAT_PERIOD         6000
#define IMMUNITY_PERIOD                 3000
#define INFECTION_REBROADCAST_PERIOD    2000

bool isLeader = false; 
bool isInfected = false;
const int BLUE_LED=6;    // leader
const int RED_LED=4;    // infected
const int GREEN_LED=5;    // clear
const int PIN_BUTTON=8;     // infection button
uint32_t immunityTimeout=millis();
uint32_t infectionRebroadcastTimeout=millis();

const uint8_t MSG_ELECTION = 0xB0,
              MSG_ACK = 0xB1,
              MSG_VICTORY = 0xB2,
              MSG_INFECTION = 0xB3,
              MSG_CLEAR = 0xB4,
              MSG_DISCOVERY = 0xB5,
              MSG_HEARTBEAT = 0xB6;

const uint8_t slCommand[] = {'S', 'L'};

XBee xbee = XBee();
SoftwareSerial xbeeSerial(2, 3);
ZBRxResponse rxResponse = ZBRxResponse();
ZBTxRequest txRequest;
AtCommandRequest atRequest = AtCommandRequest((uint8_t*)slCommand);
AtCommandResponse atResponse;

uint32_t myAddress = 0xA1,
leaderAddress64, remoteAddress64;
uint32_t listAddress64[10];
uint8_t numDevices = 0;

bool isElecting = false;
bool isAcknowledged = false;
uint32_t electionTimeout, leaderHeartbeatTimeout, betweenElectionTimeout;
uint8_t heartbeatsLost = 0;
int button_state = LOW, last_button_state = HIGH;
int debounce_timestamp = 0;
int debounce_delay = 50;

void set_Leds(bool leader,bool infection) {
  if (leader == isLeader && infection == false)   {
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else if (leader == !isLeader && infection == true){
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);}
      
  else if (leader != isLeader && infection == false)  {
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
  }

void setup() {
  Serial.begin(57600);
  xbeeSerial.begin(57600);
  xbee.begin(xbeeSerial);

  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

// Make all LED's on to indicate that we are in the setup mode
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);

Serial.println(myAddress);
leaderAddress64 = myAddress;
  sendCommand(0x0000FFFF, (uint8_t*)& MSG_DISCOVERY, 1);
  leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
}

void loop() {
  
      if (leaderAddress64 == myAddress) {
                  Serial.println("Main Loop");

          set_Leds(isLeader,isInfected);
        if (button_state == LOW) 
        {
          sendCommand(0x0000FFFF, (uint8_t*)&MSG_INFECTION, 1);
      }
        }
        else if (leaderAddress64 != myAddress){
          set_Leds(!isLeader,isInfected);
             if (button_state == LOW) 
              {
              sendCommand(0x0000FFFF, (uint8_t*) & MSG_INFECTION, 1);
              set_Leds(!isLeader,isInfected);
              Serial.println("here");
              }
            }
  int reading = digitalRead(PIN_BUTTON);
  if (reading != last_button_state) debounce_timestamp = millis();
  if (millis() - debounce_timestamp > debounce_delay) {
    if (reading != button_state) {
      button_state = reading;

    }
  }
  last_button_state = reading;
  readAndHandlePackets();
  if (isElecting && millis() > electionTimeout) {
    isElecting = false;
    leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
    betweenElectionTimeout = millis() + ELECTION_BETWEEN_WAIT_PERIOD;
    if (isAcknowledged) beginElection();
    else {
      sendCommand(0x0000FFFF, (uint8_t*)&MSG_VICTORY, 1);
      leaderAddress64 = myAddress;
    }
  }
  if (!isElecting) {
    if (millis() > leaderHeartbeatTimeout) {
      if (leaderAddress64 == myAddress) {
        sendCommand(0x0000FFFF, (uint8_t*) &MSG_HEARTBEAT, 1);
        leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD / 3;
      } else {
        beginElection();
      }
    }
    if (leaderAddress64 != myAddress && isInfected && millis() > infectionRebroadcastTimeout) {
      sendCommand(0x0000FFFF, (uint8_t*) &MSG_INFECTION, 1);
      infectionRebroadcastTimeout = millis() + INFECTION_REBROADCAST_PERIOD;
    }
  }
}


void sendCommand(uint32_t destination_Addr, uint8_t* payload, uint8_t length) {
  
  if (payload[0] == MSG_DISCOVERY || payload[0] == MSG_VICTORY) {
    txRequest = ZBTxRequest(XBeeAddress64(0x00000000, 0x0000FFFF), payload, length);
    xbee.send(txRequest);
  } else {
    if (destination_Addr == 0x0000FFFF) {
      for (int i = 0; i < numDevices; i++) {
        txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, listAddress64[i]), payload, length);
        xbee.send(txRequest);
      }
    } else {
      txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, destination_Addr), payload, length);
      xbee.send(txRequest);
    }
  }
}

void beginElection(void) {
  if (isElecting)  return;
  if (millis() < betweenElectionTimeout) return;
  else isElecting = true;
  Serial.println("Began election");
  isAcknowledged = false;
  uint8_t countDevices = 0;
  Serial.println("Candidates:");
  for (int i = 0; i < numDevices; i++) {
    Serial.println(listAddress64[i], HEX);
    if (listAddress64[i] > myAddress) {
      sendCommand(listAddress64[i], (uint8_t*) &MSG_ELECTION, 1);
      countDevices++;
    }
  }
  if (countDevices > 0) electionTimeout = millis() + ELECTION_REPLY_WAIT_PERIOD;
  else electionTimeout = millis();
}

void readAndHandlePackets(void) {
  if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
    xbee.getResponse().getZBRxResponse(rxResponse);
    remoteAddress64 = rxResponse.getRemoteAddress64().getLsb();
    //    if (remoteAddress64 > leaderAddress64) beginElection();     // VERIFY WHETHER YOU ACTUALLY NEED THIS
    

    bool inList = false;
    for (int i = 0; i < numDevices; i++)
      if (listAddress64[i] == remoteAddress64)
        inList = true;
    if (!inList) listAddress64[numDevices++] = remoteAddress64;
    switch (rxResponse.getData(0)) {
      case MSG_DISCOVERY:
        if (rxResponse.getDataLength() > 1) {
          memcpy(&leaderAddress64, rxResponse.getData() + 1, sizeof(leaderAddress64));
          if (leaderAddress64 < myAddress) beginElection();
        } else {
          uint8_t msgPayload[5];
          msgPayload[0] = MSG_DISCOVERY;
          memcpy(msgPayload + 4, &leaderAddress64, sizeof(leaderAddress64));
          sendCommand(remoteAddress64, msgPayload, 5);
        }
        break;

      case MSG_ELECTION:
        sendCommand(remoteAddress64, (uint8_t*)&MSG_ACK, 1);
        beginElection();
        break;

      case MSG_ACK:
        electionTimeout = millis() + ELECTION_VICTORY_WAIT_PERIOD;
        isAcknowledged = true;
        break;

      case MSG_VICTORY:
        if (remoteAddress64 > myAddress) {
          leaderAddress64 = remoteAddress64;
          isElecting = false;
          leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
          betweenElectionTimeout = millis() + ELECTION_BETWEEN_WAIT_PERIOD;
        }
        else beginElection();
        break;

      case MSG_HEARTBEAT:
        if (myAddress == leaderAddress64) {
          if (remoteAddress64 > myAddress){
            leaderAddress64 = remoteAddress64;
            leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
          }
        } else leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
        break;

      case MSG_INFECTION:
        //if (millis() > immunityTimeout && leaderAddress64 != myAddress)
          isInfected = true;
        //immunityTimeout = millis() + IMMUNITY_PERIOD;
        break;

      case MSG_CLEAR:
        isInfected = false;
        immunityTimeout = millis() + IMMUNITY_PERIOD;
        break;
    }
  }
}
