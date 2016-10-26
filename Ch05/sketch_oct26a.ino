
#include <Wire.h>
#include <Servo.h>
#include <math.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    LIDARLite_ADDRESS_2   0x64          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

Servo myservo;
Servo esc; //ESC can be controlled like a servo.

int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int pos = 0;                      // Position of the servo (degress, [0, 180])
int distance = 0;                 // Distance measured
int min_distance [100];           // keep track of the min distances this is can be modified later
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

void setup()
{
  Serial.begin(9600);
  Serial.println("< calibration Mode waite for 3 sec >"); 
  myservo.attach(5);   // Servo control use pin 5 for PWM to servo
  esc.attach(9); // ESC control use pin 9 for PWM to ESC
  Wire.begin(); // join i2c bus LIDAR control for both LIDAR's
  calibrate_myservo(); // making sure that the servo is in the right position

}
/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

void calibrate_myservo(){
while (Serial.available() == 0) // while there is No Xbee signal to start
  {
    esc.write(90); // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(5));            // reads the value of the potentiometer (value between 0 and 1023) 
    pos = map(pos, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
    myservo.write(pos);                  // sets the servo position according to the scaled value 
    distance_from_obstacle_1 = lidarGetRange();
    distance_from_obstacle_2 = lidarGetRange_2();
    delay(1000);
  }

}
// Get a measurement from the LIDAR Lite

int lidarGetRange(void)
{
  int val = -1;
  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting
  delay(20); // Wait 20ms for transmit
  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting
  delay(20); // Wait 20ms for transmit
  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite
  if(2 <= Wire.available()) // if two bytes were received
  {
    val = Wire.read(); // receive high byte (overwrites previous reading)
    val = val << 8; // shift high byte to be high 8 bits
    val |= Wire.read(); // receive low byte as lower 8 bits
  }
  
  return val;
}

int lidarGetRange_2(void)
{
  int va2 = -1;
  
  Wire.beginTransmission((int)LIDARLite_ADDRESS_2); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS_2); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit
  
  Wire.requestFrom((int)LIDARLite_ADDRESS_2, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    va2 = Wire.read(); // receive high byte (overwrites previous reading)
    va2 = va2 << 8; // shift high byte to be high 8 bits
    va2 |= Wire.read(); // receive low byte as lower 8 bits
  }
  
  return va2;
}
void serialPrintRange(int pos, int distance)
{
    Serial.print("Position (deg): ");
    Serial.print(pos);
    Serial.print("\t\tDistance (cm): ");
    Serial.println(distance);
}

void loop()
{

    myservo.write(pos);
    distance_from_obstacle_1 =lidarGetRange();
    distance_from_obstacle_2 =  lidarGetRange_2();
    distance = min(distance_from_obstacle_1,distance_from_obstacle_2);
    //min_distance [ii] = distance;
    //if min_distance [ii] <>min_distance [ii-1]
    // if distance_from_obstacle_1>distance_from_obstacle_2
    
    serialPrintRange(pos, distance);
    delay(20);
  
}

