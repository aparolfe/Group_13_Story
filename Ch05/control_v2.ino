/*
 * Traking all used PIN's
 * servo  PIN 5
 * ESC    PIN 9
 * lidar PIN 2,3, A4,A5
 * IR PIN A0, A1
 * Xbee PIN ?
 * 
 */

#include <Wire.h>   // for LIDAR
#include <Servo.h>
#include <math.h>
#define    INTERVAL     50      // (ms) Interval between distance readings.
//#define    BOUNDARY     42      // (cm) Avoid objects closer than 30cm.
#define    LIDARLite_ADDRESS   0x62       // Default I2C Address of LIDAR-Lite - party line
#define    RegisterMeasure     0x00       // Register to write to initiate ranging.
#define    MeasureValue        0x04       // Value to initiate ranging.
#define    RegisterHighLowB    0x8f       // Register to get both High and Low bytes in 1 call.
#define    LeftRangeSensor     2          // Pow-En Pin for LIDAR mounted on device left.
#define    RightRangeSensor    3          // Pow-En Pin for LIDAR mounted on device right.
Servo myservo;
Servo esc; //ESC can be controlled like a servo.
int   BOUNDARY = 42;      // (cm) Avoid objects closer than 30cm.
String stop_start;
int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int pos = 0;                      // Position of the servo (degress, [0, 180])
int min_distance_to_wall ;           // keep track of the min distances this is can be modified later
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int duration;
String wheel; // this is used to know which wheel is closer to the wall used string are "right_wheel" or "left_wheel" 
unsigned long pulse_widthleft;
unsigned long pulse_widthright;
// set the value of IR sensor

int distance = 0;    // Distance measured

void lidarSetup() 
{
  Wire.begin(); // join i2c bus
  pinMode(LeftRangeSensor, OUTPUT); 
  pinMode(RightRangeSensor, OUTPUT);
  delay(2);
  //Set both sensors to sleep
  digitalWrite(LeftRangeSensor, LOW);
  digitalWrite(RightRangeSensor, LOW);
}

int lidarGetDistance(int sensorNumber)   // Get a measurement from the LIDAR Lite
{
  if (sensorNumber != LeftRangeSensor && sensorNumber != RightRangeSensor) {   // Input sanity check
    Serial.println("Invalid input to lidarGetDistance");
    return -1;
  }
      
  digitalWrite(sensorNumber, HIGH);   // Wake up sensor
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
  
  digitalWrite(sensorNumber, LOW);   // Sleep mode for sensor

  // Add code here for distance corrections after sensor mounted and calibrated
  // For now, subtracting offset assuming sensors mounted in the middle of the chassis
  val = val - 20;
  return val;
}

void lidarPrintDistance(int sensorNumber, int distance)
{
    if (sensorNumber == LeftRangeSensor) 
    {
      Serial.print("Reading from Left LIDAR - ");
      Serial.print("\t\t\t\tDistance (cm): ");
      Serial.println(distance);
    }
    else if (sensorNumber == RightRangeSensor) 
    {
      Serial.print("Reading from Right LIDAR - ");
      Serial.print("\tDistance (cm): ");
      Serial.println(distance);
    }
    else {
      Serial.println("Invalid input to lidarPrintDistance");
    }
}


void setup()
{
  Serial.begin(9600);
   stop_start = "0"; //stop
  Serial.println("< calibration Mode started waite for 3 sec >");
  lidarSetup();
  myservo.attach(5);   // Servo control use pin 5 for PWM to servo
  esc.attach(9); // ESC control use pin 9 for PWM to ESC
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

void forward()
{
  myservo.write(90);
  esc.write(60);
  delay(100);
  Serial.println("forward Mode");
}

void leftTurn(int turnDegree)
  {
    esc.write(70); // reduce the speed
    myservo.write(90 + turnDegree); // update tge servo
    Serial.println("leftTurn Mode");
    delay(duration);
  }

void rightTurn(int turnDegree)
  {
    esc.write(70);// reduce the speed
    myservo.write(90 + turnDegree); // update tge servo
    Serial.println("rightTurn Mode");
    delay(duration);
  }
void calibrate_myservo(){

  Serial.println("<===waiting for Xbee signal to start===>");
  
while ( Serial.available()==0) // while there is No Xbee signal to start MTA:
  {
    esc.write(90); // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(5));  // reads the value of the potentiometer (value between 0 and 1023) 
    pos = map(pos, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
    myservo.write(90);                  // sets the servo position according to the scaled value 
    Serial.println("inside the calibrate_mode");
    
    // getting the both distances 
    distance_from_obstacle_1 = 50;//lidarGetDistance(LeftRangeSensor);
    distance_from_obstacle_2 = 50;//lidarGetDistance(RightRangeSensor);
    min_distance_to_wall = min (distance_from_obstacle_1,distance_from_obstacle_2); // keep track of the min distances and store that in the array 
    
    Serial.println("min_distance_to_wall"); 
    Serial.println(min_distance_to_wall);
    delay(1000);
  }

  if ( Serial.available()>0)
  {stop_start = "1";}
  delay (100);
}

void loop()
{
 int ii=0;  //local conter for the min distances array
 forward(); // car moves forward continuously.
 
 // loop note : this loop will be used if the car far from the wall
 
  do 

   {         
       distance_from_obstacle_1 = 50;//lidarGetDistance(LeftRangeSensor);
       distance_from_obstacle_2 = 50;//lidarGetDistance(RightRangeSensor);
       min_distance_to_wall = min (distance_from_obstacle_1,distance_from_obstacle_2); // keep track of the min distances and store that in the array 
       
       Serial.println(min_distance_to_wall);       // Print it out.
    if (distance_from_obstacle_1 < distance_from_obstacle_2) // keep track of the closer wheel to the wall
      {wheel = "left_wheel";}
      else
      {wheel = "right_wheel";}
        
//      if (ii == 10) // if the counter reached the max reset it and copy the last read min distance 
//          { 
//           // min_distance_to_wall[0] =  min_distance_to_wall[10];
//          ii = 1;
//          }
//
     delay(1000);
     if (Serial.available()>0)
       {stop_start = Serial.readString();}
       
//       ii++;
    }
 while(min_distance_to_wall >= BOUNDARY &&  stop_start == "1");//loop til an object is sensed MTA:

//=======================================================================

//This loop will be used if the car is closer to the right wall then the car should trun left
// or 
// This loop will be used if the car is closer to the left wall then the car should trun right
  
 while(min_distance_to_wall < BOUNDARY && stop_start == "1")
   { 
            Serial.println("Test Loop2");       // Print it out.

            double rad = degToRad(ii);
            double wheelOffset = sin(rad) * maxWheelOffset;
            Serial.println(BOUNDARY);
            if (wheel == "left_wheel")
            {Serial.println("close to left wall"); 
            
            rightTurn((-1)*wheelOffset); // calling trun right function
            ii++; //degree counter
            Serial.println("calling rightTurn() function");
            delay(INTERVAL);                // Delay between readings.
            }
            
            else if (wheel == "right_wheel")
            {Serial.println("closer to right wall"); 
            //Serial.println(min_distance_to_wall); 
            leftTurn(wheelOffset); // calling trun right function
            ii++; //degree counter
            delay(INTERVAL);                // Delay between readings.
            Serial.println("calling rightTurn() function");
            }
            if (Serial.available()>0)
       {stop_start = Serial.readString();
       //min_distance_to_wall = 50;//lidarGetDistance(LeftRangeSensor);

       }
                             
    }
 //======================================================================
 //??(should be in the loop of turn) to make sure the car is going straight forward() function is called in the next loop to recover the added/subtracted angles
 // when read stop from Xbee, stop
 do{
    esc.write(90); // MTA: 90 means stop
    if (Serial.available()>0)
       {stop_start = Serial.readString();
       break;
     }   
   }
 while( stop_start == "0");
 //when the IR sensor read something, stop
 //do{
  //esc.write(90);
 //}
 //while(true); 
}
