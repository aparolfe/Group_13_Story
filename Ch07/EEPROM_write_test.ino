// Code adapted from http://forum.arduino.cc/index.php?topic=45104.0

// Write the Serial of the Arduino in the first 2 bytes of the EEPROM

#include <EEPROM.h>
char sID[3] = "A5";

void setup ()
{
  Serial.begin (9600);
  for (int i=0; i<2; i++) {
    EEPROM.write(i,sID[i]);
  }
  Serial.println("Written"); 
  Serial.println(sID); 
  for (int i=0; i<2; i++) {
    sID[i] = EEPROM.read(i);
  }
  Serial.println("Read");
  Serial.println(sID);
}

void loop () {
}
