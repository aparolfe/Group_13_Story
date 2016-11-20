#include <XBee.h>
#include <SoftwareSerial.h>

XBee xbee = XBee();
SoftwareSerial xbeeSerial(2, 3);
ZBRxResponse rxResponse = ZBRxResponse();
ZBTxRequest txRequest;

const int BLUE_LED=6;    // leader
const int RED_LED=4;    // infected
const int GREEN_LED=5;    // clear
const int PIN_BUTTON=8;     // infection button

bool leader = true;
bool infected = false;
int wait_for_Leader_heart_beat = 4000;

uint32_t immunityTimeout=millis();
uint32_t infectionRebroadcastTimeout=millis();
uint32_t myID = 0xA1;

const uint8_t Massage,
              IsthereLeader = 0xC1,
              HeartBeat_Reply = 0xC2,
              Clear_Infection = 0xC3,
              Infection_to_all = 0xC3;



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
  delay(2000);

  Serial.println(myAddress); // send my address to all
  
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

void set_Leds(bool isleader,bool isinfected) {
  if (isleader==true  && isinfected == false || isleader==true  && isinfected == true)   {
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else if (isLeader==false && isinfected == true){
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);}
      
  else if (isLeader==false && isinfected == false)  {
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
  }

//========================== Send API Masseges==================== 

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

//=================== Read from API ==========================


//============================================================

//============= Election Method================
void Electing(void)
{
   sendCommand(0x0000FFFF, (uint8_t*) & myID, 1);

 }
//=============================================
void loop() {
  
}

    xbee.getResponse().getZBRxResponse(rxResponse);
    Massage = rxResponse.getData(0);

      if (leaderAddress == myAddress) {
          set_Leds(isLeader,isInfected);
          sendCommand(0x0000FFFF, (uint8_t*) & HeartBeat_Reply, 1);
        if (button_state == LOW) 
         {
          sendCommand(0x0000FFFF, (uint8_t*) & Clear_Infection, 1);
         }
        }
        else if (leaderAddress != myAddress){
          set_Leds(!isLeader,isInfected);
             if (button_state == LOW) 
              {
                sendCommand(0x0000FFFF, (uint8_t*) & Infection_to_all, 1);
                set_Leds(!isLeader,isInfected);
              }
            }
    }



