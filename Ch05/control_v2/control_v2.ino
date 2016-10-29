/*
 * Traking all used PIN's
 * servo  PIN 5
 * ESC    PIN 9
 * lidar PIN 2,3, A4,A5
 * IR PIN A0, A1
 * Xbee PIN ?
 * 
 */


#include <Servo.h>
#include <math.h>
#define    INTERVAL     50      // (ms) Interval between distance readings.
//#define    BOUNDARY     42      // (cm) Avoid objects closer than 30cm.

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
unsigned long pulse_width;
unsigned long pulse_width2;
// set the value of IR sensor

int distance = 0;    // Distance measured



int lidarGetDistance()   // Get a measurement from the LIDAR Lite
{
    pulse_width = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
   if(pulse_width != 0){ // If we get a reading that isn't zero, let's print it
    pulse_width = pulse_width/10; // 10usec = 1 cm of distance for LIDAR-Lite
    Serial.print("left:");
    Serial.println(pulse_width); // Print the distance
    return pulse_width; 
    delay(50);
  }
}

int lidarGetDistance_2()   // Get a measurement from the LIDAR Lite
{
   pulse_width2 = pulseIn(6, HIGH); // Count how long the pulse is high in microseconds
  if(pulse_width2 != 0){ // If we get a reading that isn't zero, let's print it
        pulse_width2 = pulse_width2/10; // 10usec = 1 cm of distance for LIDAR-Lite
    Serial.print("right:");
    Serial.println(pulse_width2); // Print the distance
    return pulse_width2;
    delay(50); 
  } 
  
}




void setup()
{ 
  Serial.begin(9600);
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  pinMode(4, OUTPUT); // Set pin 2 as trigger pin
  pinMode(6, INPUT); // Set pin 3 as monitor pin
  digitalWrite(4, LOW); // Set trigger LOW for continuous read
  stop_start = "0"; //stop
  Serial.println("< calibration Mode started waite for 3 sec >");
  
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
    distance_from_obstacle_1 = lidarGetDistance();
    distance_from_obstacle_2 = lidarGetDistance_2();
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
       distance_from_obstacle_1 = lidarGetDistance();
       distance_from_obstacle_2 = lidarGetDistance_2();
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
            distance_from_obstacle_1 = lidarGetDistance();
            distance_from_obstacle_2 = lidarGetDistance_2();
            min_distance_to_wall = min (distance_from_obstacle_1,distance_from_obstacle_2); // keep track of the min distances and store that in the array 
       
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
