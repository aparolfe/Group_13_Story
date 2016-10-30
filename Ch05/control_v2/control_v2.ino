/*
   Tracking all used PINs
   servo  PIN 5
   ESC    PIN 9
   lidar PIN 2,3,4,6
   IR PIN A0, A1
   Xbee PIN Rx, Tx

*/

#include <Servo.h>
#include <math.h>
#define    INTERVAL     50      // (ms) Interval between distance readings.
//#define    BOUNDARY     42      // (cm) Avoid objects closer than 30cm.

//Tracking all used pins
#define LeftRangeTrigger      2   // Trigger Pin for LIDAR mounted on device left.
#define LeftRangeSensor       3   // Monitor Pin for LIDAR mounted on device left.
#define RightRangeTrigger     4   // Trigger Pin for LIDAR mounted on device right.
#define RightRangeSensor      6   // Monitor Pin for LIDAR mounted on device right.
#define FrontCollisionSensor  A0  // Front SharpIR sensor
#define BackCollisionSensor   A1  // Back SharpIR sensor
#define ServoPin              5   // Servo to control wheel turn (PWM)
#define EscPin                9   // Speed control (PWM)

Servo myservo;
Servo esc;              //ESC can be controlled like a servo.
int   BOUNDARY = 42;    // (cm) Avoid objects closer than 30cm.
String stop_start;
int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int pos = 0;                      // Position of the servo (degress, [0, 180])
int min_distance_to_wall ;        // keep track of the min distances this is can be modified later
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int duration;
String wheel; // this is used to know which wheel is closer to the wall used string are "right_wheel" or "left_wheel"
unsigned long pulse_width;
// set the value of IR sensor

void lidarSetup()
{ // Set up trigger and monitor pins, set trigger pins to low for continuous read
  pinMode(LeftRangeTrigger, OUTPUT);
  pinMode(LeftRangeSensor, INPUT);
  digitalWrite(LeftRangeTrigger, LOW);
  pinMode(RightRangeTrigger, OUTPUT);
  pinMode(RightRangeSensor, INPUT);
  digitalWrite(RightRangeTrigger, LOW);
}

int lidarGetDistance(int sensorNumber)   // Get a measurement from the LIDAR Lite
{
  if (sensorNumber != LeftRangeSensor && sensorNumber != RightRangeSensor)
  { // Input sanity check
    Serial.println("Invalid input to lidarGetDistance");
    return -1;
  }
  pulse_width = pulseIn(sensorNumber, HIGH); // Count how long the pulse is high in microseconds
  if (pulse_width != 0)
  { // If we get a reading that isn't zero, let's print it
    pulse_width = pulse_width / 10; // 10usec = 1 cm of distance for LIDAR-Lite
    if (sensorNumber == LeftRangeSensor) Serial.print("left:");
    else Serial.print("right:");
    Serial.println(pulse_width); // Print the distance
    return pulse_width;
    delay(50);
  }
}

void setup()
{
  Serial.begin(9600);
  stop_start = "0"; //stop
  Serial.println("< calibration Mode started wait for 3 sec >");
  lidarSetup();
  myservo.attach(ServoPin);   // Servo setup
  esc.attach(EscPin);         // ESC setup
  calibrate_myservo(); // make sure that the servo is in the right position


}
/* Convert degree value to radians */
double degToRad(double degrees) {
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians) {
  return (radians * 4068) / 71;
}

void forward()
{
  myservo.write(90);
  esc.write(60);
  delay(100);
  Serial.println("forward Mode");
}

void turn(String dir, int turnDegree)
{
  esc.write(70); // reduce the speed
  if (dir == "Left") {
    myservo.write(90 + turnDegree); // update the servo
    Serial.println("leftTurn Mode");
  }
  if (dir == "Right") {
    myservo.write(90 - turnDegree); // update the servo
    Serial.println("rightTurn Mode");
  }
  delay(duration);
}
/*
  void leftTurn(int turnDegree)
  {
  esc.write(70); // reduce the speed
  myservo.write(90 + turnDegree); // update the servo
  Serial.println("leftTurn Mode");
  delay(duration);
  }

  void rightTurn(int turnDegree)
  {
  esc.write(70);// reduce the speed
  myservo.write(90 + turnDegree); // update the servo
  Serial.println("rightTurn Mode");
  delay(duration);
  }
*/
void calibrate_myservo() {
  Serial.println("<===waiting for Xbee signal to start===>");
  while ( Serial.available() == 0) // while there is No Xbee signal to start MTA:
  {
    esc.write(90); // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(ServoPin));  // reads the value of the potentiometer (value between 0 and 1023)
    pos = map(pos, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180)
    myservo.write(90);                  // sets the servo position according to the scaled value
    Serial.println("inside the calibrate_mode");

    // getting the both distances
    distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
    distance_from_obstacle_2 = lidarGetDistance(RightRangeSensor);
    min_distance_to_wall = min (distance_from_obstacle_1, distance_from_obstacle_2); // keep track of the min distances and store that in the array

    Serial.println("min_distance_to_wall");
    Serial.println(min_distance_to_wall);
    delay(1000);
  }

  if ( Serial.available() > 0)
  {
    stop_start = Serial.readString();
  }
  delay (100);
}

void coast() { // used if the car far from the wall
  distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
  distance_from_obstacle_2 = lidarGetDistance(RightRangeSensor);
  min_distance_to_wall = min (distance_from_obstacle_1, distance_from_obstacle_2); // keep track of the min distances and store that in the array

  Serial.println(min_distance_to_wall);       // Print it out.
  if (distance_from_obstacle_1 < distance_from_obstacle_2) // keep track of the closer wheel to the wall
  {
    wheel = "left_wheel";
  }
  else
  {
    wheel = "right_wheel";
  }
  delay(1000);
  if (Serial.available() > 0)
  {
    stop_start = Serial.readString();
  }
}

void swerve(int angle) {    // used if the car is too close to a wall and should turn away from the wall
  Serial.println("Test Loop2");       // Print it out.
  distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
  distance_from_obstacle_2 = lidarGetDistance(RightRangeSensor);
  min_distance_to_wall = min (distance_from_obstacle_1, distance_from_obstacle_2); // keep track of the min distances and store that in the array

  double rad = degToRad(angle);
  double wheelOffset = sin(rad) * maxWheelOffset;
  Serial.println(BOUNDARY);
  if (wheel == "left_wheel")
  { Serial.println("close to left wall");
    turn("RIGHT", wheelOffset);   //  turn right
    Serial.println("calling rightTurn() function");
  }
  else if (wheel == "right_wheel")
  { Serial.println("close to right wall");
    //Serial.println(min_distance_to_wall);
    turn("LEFT", wheelOffset);      //  turn left
    Serial.println("calling leftTurn() function");
  }
  delay(INTERVAL);                // Delay between readings.
  if (Serial.available() > 0)
  { stop_start = Serial.readString();
  }
}

void loop()
{
  int ii = 0; // local counter for the min distances array
  forward();  // car moves forward continuously.

  do
  {
    coast();
  }
  while (min_distance_to_wall >= BOUNDARY &&  stop_start == "1"); //loop til an object is sensed MTA:

  //=======================================================================
  // or
  // This loop will be used if the car is closer to the left wall then the car should turn right

  while (min_distance_to_wall < BOUNDARY && stop_start == "1")
  {
    swerve(ii);
    ii++; //degree counter
  }
  //======================================================================
  //??(should be in the loop of turn) to make sure the car is going straight forward() function is called in the next loop to recover the added/subtracted angles
  // when read stop from Xbee, stop
  do {
    esc.write(90); // MTA: 90 means stop
    if (Serial.available() > 0)
    { stop_start = Serial.readString();
      break;
    }
  }
  while ( stop_start == "0");
  //when the IR sensor read something, stop
  //do{
  //esc.write(90);
  //}
  //while(true);
}
