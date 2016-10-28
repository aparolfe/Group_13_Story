/*
 * Traking all used PIN's
 * servo  PIN 5
 * ESC    PIN 9
 * lidarleft PIN 2,3
 * lidarright PIN 4,6
 * IR PIN ? ?
 * Xbee PIN ?
 */



#include <Servo.h>
#include <math.h>
#define    INTERVAL     25      // (ms) Interval between distance readings.
#define    BOUNDARY     40 // (cm) Avoid objects closer than 30cm.

Servo myservo;
Servo esc; //ESC can be controlled like a servo.

int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int pos = 0;                      // Position of the servo (degress, [0, 180])
int distance = 0;                 // Distance measured
//int min_distance           // keep track of the min distances this is can be modified later
int min_distance_to_wall [100];           // keep track of the min distances this is can be modified later
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int duration;
String wheel; // this is used to know which wheel is closer to the wall used string are "right_wheel" or "left_wheel" 
unsigned long pulse_widthleft;
unsigned long pulse_widthright;
// set the value of IR sensor
void setup()
{
  Serial.begin(9600);
  Serial.println("< calibration Mode started waite for 3 sec >");
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  pinMode(4, OUTPUT); // Set pin 4 as trigger pin
  pinMode(6, INPUT); // Set pin 6 as monitor pin
  digitalWrite(4, LOW); // Set trigger LOW for continuous read 
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
  esc.write(25);
  delay(100);
}

void leftTurn(int turnDegree)
  {
    esc.write(15); // reduce the speed
    myservo.write(90 + turnDegree); // update tge servo
    Serial.println("Turning Left with totall offset of :");
    Serial.println(90.0+turnDegree);
    delay(duration);
  }

void rightTurn(int turnDegree)
  {
    esc.write(15);// reduce the speed
    myservo.write(90 + turnDegree); // update tge servo
    Serial.println("Turning Right with offset  of :");
    Serial.println(90 + turnDegree);
    delay(duration);
  }
void calibrate_myservo(){

  Serial.println("waiting for Xbee signal to start");
  
while (Serial.readString() = "start") // while there is No Xbee signal to start
  {
    esc.write(90); // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(5));  // reads the value of the potentiometer (value between 0 and 1023) 
    pos = map(pos, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
    myservo.write(pos);                  // sets the servo position according to the scaled value 
    Serial.println("servor scaled position is"+ pos);
    
    // getting the both distances
    distance_from_obstacle_1 = lidarGetRange();
    distance_from_obstacle_2 = lidarGetRange_2();
    delay(1000);
  }
delay (1000);
}
// Get a measurement from the LIDAR Lite

int lidarGetRange(void)
{
  pulse_widthleft = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  if(pulse_widthleft != 0){ // If we get a reading that isn't zero, let's print it
     pulse_widthleft = pulse_widthleft/10; // 10usec = 1 cm of distance for LIDAR-Lite
  }
  delay(20);
}

int lidarGetRange_2(void)
{
  pulse_widthright = pulseIn(6, HIGH); // Count how long the pulse is high in microseconds
  if(pulse_widthright != 0){
   // If we get a reading that isn't zero, let's print it
     pulse_widthright = pulse_widthright/10; // 10usec = 1 cm of distance for LIDAR-Lite
  }
  delay(20);
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

 int ii=0; //local conter for the min distances array
 forward();// car moves forward continuously.

 // loop note : this loop will be used if the car far from the wall
  do 
   {
      min_distance_to_wall[ii] = min (lidarGetRange(),lidarGetRange_2()); // keep track of the min distances and store that in the array  
      if (lidarGetRange()<lidarGetRange_2()) // keep track of the closer wheel to the wall
      {wheel = "left_wheel";}
      else
      {wheel = "right_wheel";}
      Serial.println(min_distance_to_wall[ii]);       // Print it out.  
      delay(INTERVAL);// Delay between readings.   
      ii++;
      
      if (ii == 100) // if the counter reached the max reset it and copy the last read min distance 
          {min_distance_to_wall[1] = min_distance_to_wall[100];
          ii = 2;
          }     
    }
 while(min_distance_to_wall[ii] >= BOUNDARY);//loop til an object is sensed

//=======================================================================

//This loop will be used if the car is closer to the right wall then the car should trun left
// or 
// This loop will be used if the car is closer to the left wall then the car should trun right
  do 
   { 
            
            double rad = degToRad(ii);
            double wheelOffset = sin(rad) * maxWheelOffset;

            if (wheel == "left_wheel")
            {Serial.println(" we need to trun the car right, wall is closer to the left wall"); 
            rightTurn((-1)*wheelOffset); // calling trun right function
            ii++; //degree conuter
            delay(INTERVAL);                // Delay between readings.
            Serial.println("calling rightTurn() function");}
            
            else if (wheel == "right_wheel")
            {Serial.println(" we need to trun the car left, wall is closer to the right wall"); 
            leftTurn(wheelOffset); // calling trun right function
            ii++; //degree conuter
            delay(INTERVAL);                // Delay between readings.
            Serial.println("calling rightTurn() function");}
                             
    }
 while(min_distance_to_wall[ii] < BOUNDARY);
 //======================================================================
 //??(should be in the loop of turn) to make sure the car is going straight forward() function is called in the next loop to recover the added/subtracted angles
 // when read stop from Xbee, stop
 do{
  esc.write(0);
 }
 while(Serial.readString()="stop");
 //when the IR sensor read something, stop
 do{
  esc.write(0);
 }
 while(true);
}
