#include <MsTimer2.h>
#include <Servo.h>
#include <math.h>
#include <PID_v1.h>

#define INTERVAL      50      // (ms) Interval between distance readings.
#define BOUNDARY      98      // Avoid objects closer than 30cm.
#define IR_THRESHOLD  160     // Max acceptable reading of front collision sensor
#define IR_THRESHOLD_2  180     // Max acceptable reading of front collision sensor
#define ESC_STOP      90      // Neutral ("Stop") value for ESC

// Tracking all used pins
#define LeftRangeTrigger      3   // Trigger Pin for LIDAR mounted on device left.
#define LeftRangeSensor       7  // Monitor Pin for LIDAR mounted on device left.
#define RightRangeTrigger     4   // Trigger Pin for LIDAR mounted on device right.
#define RightRangeSensor      6   // Monitor Pin for LIDAR mounted on device right.
#define FrontCollisionSensor  A0  // Front SharpIR sensor
#define BackCollisionSensor   A1  // Back SharpIR sensor
#define ServoPin              9   // Servo to control wheel turn (PWM)
#define EscPin                5   // Speed control (PWM)

Servo myservo;
Servo esc;              // ESC can be controlled like a servo.

// Global variables
char stop_start;        // Store latest xbee command
int safety_check;       // Store latest collision sensor reading
int safety_check_2;     // Store latest collision sensor reading

// LIDAR
int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int min_distance_to_wall ;        // keep track of the min distance
// Servo
int pos = 0;                      // Position of the servo (degress, [0, 180])
int wheelNeutral = 115;      // neutral wheel position, in servo 'degrees'

//ESC

int maxSpeedOffset = 73; // maximum speed magnitude, in servo 'degrees'
int minSpeedOffset = 78; // minimum speed magnitude, in servo 'degrees'
int minSpeedbackward = 95;
int maxSpeedbackward =100;
int minSpeedOffset_1;

int currentSpeedOffset = ESC_STOP; // start with speed 0
int linearAcc = 4; // Rate of speed change
//set PID parameters
double Input_servo, Output_servo;
double Input_speed, Output_speed;
double Setpoint = 98;

int Kp_servo = 1.7;
int Ki_servo = 0.04;
int Kd_servo = 0.5;
int Kp_speed = 2;
int Ki_speed = 0.05;
int Kd_speed = 0.5;
PID myPID_servo(&Input_servo, &Output_servo, &Setpoint, Kp_servo, Ki_servo, Kd_servo, DIRECT);
PID myPID_speed(&Input_speed, &Output_speed, &Setpoint, Kp_speed, Ki_speed, Kd_speed, DIRECT);

String wheel; // Store which wheel is closer to a wall, used strings are "right_wheel" or "left_wheel"
unsigned long pulse_width;

void irSetup() // Set up signal pins
{
  pinMode(FrontCollisionSensor, INPUT);
  pinMode(BackCollisionSensor, INPUT);
}

int irGetDistance(char sensorId) // Get a measurement from the Sharp IR sensor
{
  int reading = 0;
  if (sensorId == FrontCollisionSensor)
  {
    reading = analogRead(FrontCollisionSensor);
    Serial.print("Reading from Front IR - ");
    Serial.println(reading);
  }
  else if (sensorId == BackCollisionSensor)
  {
    reading = analogRead(BackCollisionSensor);
    Serial.print("Reading from Back IR - ");
    Serial.println(reading);
  }
  else
  {
    Serial.print("Invalid input to irGetDistance");
  }
  return reading;
}

void lidarSetup()   // Set up trigger and monitor pins, set trigger pins to low for continuous read
{
  pinMode(LeftRangeTrigger, OUTPUT);
  pinMode(LeftRangeSensor, INPUT);
  digitalWrite(LeftRangeTrigger, LOW);
  pinMode(RightRangeTrigger, OUTPUT);
  pinMode(RightRangeSensor, INPUT);
  digitalWrite(RightRangeTrigger, LOW);
}
void PIDSetup()
{
  myPID_servo.SetOutputLimits(0, 90);
  myPID_servo.SetMode(AUTOMATIC);
//  myPID_servo.SetOutputLimits(90, 45);
//  myPID_servo.SetMode(AUTOMATIC);
}

int lidarGetDistance(int sensorNumber)   // Get a measurement from the LIDAR Lite
{
  if (sensorNumber != LeftRangeSensor && sensorNumber != RightRangeSensor) // Input sanity check
  {
    Serial.println("Invalid input to lidarGetDistance");
    return -1;
  }
  pulse_width = pulseIn(sensorNumber, HIGH);  // Count how long the pulse is high in microseconds
  if (pulse_width != 0)                       // If non-zero reading, proceed
  {
    pulse_width = pulse_width / 10;           // 10usec = 1 cm of distance for LIDAR-Lite
    if (sensorNumber == LeftRangeSensor) Serial.print("left:");
    else Serial.print("right:");
    Serial.println(pulse_width);              // Print the distance
    return pulse_width;
    delay(1);
  }
}

void calibrate_myservo()
{
  Serial.println("<===waiting for Xbee signal to start===>");
  while ( stop_start == '0') // while there is No Xbee signal to start MTA:
  {
    esc.write(ESC_STOP);                  // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(ServoPin));  // reads the value of the potentiometer (value between 0 and 1023)
    pos = map(pos, 0, 1023, 0, 179);      // scale it to use it with the servo (value between 0 and 180)
    myservo.write(wheelNeutral);                    // sets the servo position according to the scaled value
    Serial.println("inside the calibrate_mode");

    // getting both distances
    distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
    distance_from_obstacle_2 = lidarGetDistance(RightRangeSensor);
    min_distance_to_wall = min (distance_from_obstacle_1, distance_from_obstacle_2);

    Serial.println("min_distance_to_wall");
    Serial.println(min_distance_to_wall);
    delay(1000);
  }
}

void timerSetup() {
  MsTimer2::set(1000, check); // set(period in ms, function name)
  MsTimer2::start();
}

void check() {  // Interrupt to get xbee and collision sensor updates
  Serial.println("X-bee check");
  if (Serial.available() > 0)
  {
    stop_start = Serial.read();
    Serial.println("Received update from x-bee");
    Serial.println(stop_start);
  }
  Serial.println("Collision sensor check");
  safety_check = irGetDistance(FrontCollisionSensor);
  safety_check_2 = irGetDistance(BackCollisionSensor);
  Serial.println("safety 1"+safety_check);
  Serial.println("safety 2"+safety_check_2);

}


void turn_left_90_degree()
{
    esc.write(75);
    myservo.write(180); // update the servo
    
}
void setup()
{
  Serial.begin(9600);
  stop_start = '0'; //stop
  Serial.println("< Calibration Mode started wait for 3 sec >");
  irSetup();
  timerSetup();
  lidarSetup();
  PIDSetup();
  myservo.attach(ServoPin);   // Servo setup
  esc.attach(EscPin);         // ESC setup
  calibrate_myservo();        // make sure that the servo is in the right position

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
  turn();
  if (distance_from_obstacle_1 > 800){
    delay(100);
    distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
    if(distance_from_obstacle_1 > 700){
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");

    esc.write(ESC_STOP);    // abrupt stop
    delay(500);
    esc.write(105);
    delay(2500);
    turn_left_90_degree();
    delay(6500);
  
    }
  }
  //speed up
//  if (currentSpeedOffset > maxSpeedOffset) {
//    currentSpeedOffset = currentSpeedOffset - linearAcc;
//    esc.write(currentSpeedOffset);
//  }
  esc.write(maxSpeedOffset);
  delay(INTERVAL);
  Serial.println("forward Mode");
}

void turn()
{
   //reduce the speed
//  if (currentSpeedOffset < minSpeedOffset) {
//    currentSpeedOffset++;
//    esc.write(minSpeedOffset);
//  }
//  myPID_speed.Compute();
//  esc.write(wheelNeutral + Output_speed); // update the esc
  Input_servo = min_distance_to_wall;
  myPID_servo.Compute();
  myservo.write(wheelNeutral + Output_servo); // update the servo
  if (distance_from_obstacle_1 > 700){
    return;
  }
  if (distance_from_obstacle_1 < distance_from_obstacle_2) // keep track of the closer wheel to the wall
  {
    Serial.println("leftTurn Mode");
    myservo.write(wheelNeutral - Output_servo); // update the servo
      esc.write(minSpeedOffset);
  }
  else
  {
    Serial.println("rightTurn Mode");
    myservo.write(wheelNeutral + Output_servo); // update the servo
      esc.write(minSpeedOffset);
  }
  delay(1);
}

void track_wall()  // used if the car far from the walls
{
  distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
  distance_from_obstacle_2 = lidarGetDistance(RightRangeSensor);
  min_distance_to_wall = min(distance_from_obstacle_1, distance_from_obstacle_2); // keep track of the min distances and store that in the array

  Serial.println(min_distance_to_wall);       // Print it out.
  if (distance_from_obstacle_1 < distance_from_obstacle_2) // keep track of the closer wheel to the wall
  {
    wheel = "left_wheel";
  }
  else
  {
    wheel = "right_wheel";
  }
  delay(50);
}

void swerve()    // used if the car is too close to a wall and should turn away from the wall
{ 
    if (distance_from_obstacle_1 > 800){
    delay(100);
    distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
    if(distance_from_obstacle_1 > 700){
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");
    Serial.println("about to turn");

    esc.write(ESC_STOP);    // abrupt stop
    delay(500);
    esc.write(105);
    delay(4000);
    turn_left_90_degree();
    delay(6500);
  
    }
  }
  Serial.println("Swerving");
  track_wall();
  if (wheel == "left_wheel")
  {
    Serial.println("close to left wall");
    turn();   // turn right
  }
  else if (wheel == "right_wheel")
  { Serial.println("close to right wall");
    turn();      // turn left
  }
  delay(INTERVAL);                // Delay between readings.
}

void loop()
{
  //int turn_angle = 0;       // local counter for turn degree
  distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
  forward();                // car moves forward continuously
  while (stop_start == '1' && safety_check <= IR_THRESHOLD && min_distance_to_wall >= BOUNDARY && distance_from_obstacle_1 < 700) // if far enough from both walls, keep going
  {
    forward();
    track_wall();
    //distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
//    while(safety_check >= IR_THRESHOLD){
//    esc.write(ESC_STOP);    // abrupt stop
//    Serial.println("AUTO Stopped to avoid foward collision");
//    delay(200);
//    }

  }
  while (stop_start == '1' && safety_check <= IR_THRESHOLD && min_distance_to_wall < BOUNDARY && distance_from_obstacle_1 < 700) // if too close to a wall, turn away from wall
  {
    //distance_from_obstacle_1 = lidarGetDistance(LeftRangeSensor);
    swerve();
//    while(safety_check >= IR_THRESHOLD){
//    esc.write(ESC_STOP);    // abrupt stop
//    Serial.println("AUTO Stopped to avoid foward collision");
//    delay(200);
//    }
  }
//  while (stop_start == '1' && safety_check <= IR_THRESHOLD && distance_from_obstacle_1 > 700)
//  {
//    esc.write(ESC_STOP);    // abrupt stop
//    delay(2000);
//    Serial.println("about to turn");
//    turn_left_90_degree();
//    delay(2000);
//  }
  while (stop_start == '0')
  {
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped because of x-bee command");
    delay(200);
  }


 //=============== Manual Mode =============================
 
    while (stop_start == 'w' )//z
   // && safety_check <= IR_THRESHOLD)

  {
    

    myservo.write(wheelNeutral); 

    while(safety_check >= IR_THRESHOLD){
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped to avoid foward collision");
    delay(2000);
    Serial.println("about to back");
    Serial.println("about to back");
    Serial.println("about to back");
    Serial.println("about to back");
    esc.write(100);
    delay(2000);
    esc.write(ESC_STOP);
    delay(2000);

    
    }
    delay(200);
    Serial.println("forward");
    minSpeedOffset_1 = minSpeedOffset;
    if (minSpeedOffset_1 > maxSpeedOffset)
    {
      minSpeedOffset_1 --;
        Serial.println(minSpeedOffset_1);
    esc.write(minSpeedOffset_1 + 8);

    }
    else
    {
    minSpeedOffset_1  = maxSpeedOffset;
    esc.write(minSpeedOffset_1 + 8);
    
    }
  }
    while (stop_start == 'z'){
    myservo.write(wheelNeutral); 
    esc.write(100);  
     delay(2000);  
    Serial.println("backward");
    
    while(safety_check_2 >= IR_THRESHOLD){
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped to avoid foward collision");
    delay(2000);
    Serial.println("about to fffback");
    Serial.println("about to fffback");
    Serial.println("about to fffback");
    Serial.println("about to fffback");
    esc.write(80);
    delay(2000);
    esc.write(ESC_STOP);
    delay(2000);
    }
    
//    if (minSpeedbackward < maxSpeedbackward)
//    {
//      minSpeedbackward ++;
//        Serial.println(minSpeedbackward);
//    }
//    else
//    {
//    minSpeedbackward  = maxSpeedbackward;
//    }
//    minSpeedOffset++;
  }
      while (stop_start == 'a' && safety_check <= IR_THRESHOLD)
  {
    esc.write(minSpeedOffset +10);
    myservo.write(wheelNeutral + 40); 
    while(safety_check >= IR_THRESHOLD){
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped to avoid left collision");
    delay(200);
    }
    Serial.println("left man");
    Serial.println("left");
    delay(200);
  }
    while (stop_start == 'd' && safety_check <= IR_THRESHOLD)
  {
    esc.write(minSpeedOffset +10);
    while(safety_check >= IR_THRESHOLD){
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped to avoid right collision");
    delay(200);
    }
    myservo.write(wheelNeutral - 40); 
    Serial.println("right");
    delay(200);
  }

  
/////////////////////////////////////////////////////////
  //========================================================
  
//  while (safety_check > IR_THRESHOLD )//|| safety_check_2 > IR_THRESHOLD)
//{
//    esc.write(ESC_STOP);    // abrupt stop
//    Serial.println("Stopped to avoid imminent collision");
//    delay(200);
//  }
}
