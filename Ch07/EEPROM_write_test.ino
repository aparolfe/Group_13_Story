
// WRITES TO SERIAL EEPROM Arduino
//
// Do this only eleven on an Arduino,
// Write the Serial of the Arduino in the
// First 6 bytes of the EEPROM

#include <EEPROM.h>
char sID[7] = "A5";

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
