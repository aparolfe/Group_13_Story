#include <MsTimer2.h>
#include <Servo.h>
#include <math.h>

#define INTERVAL      50      // (ms) Interval between distance readings.
#define BOUNDARY      60      // Avoid objects closer than 30cm.
#define IR_THRESHOLD  180     // Max acceptable reading of front collision sensor
#define ESC_STOP      90      // Neutral ("Stop") value for ESC

// Tracking all used pins
#define LeftRangeTrigger      2   // Trigger Pin for LIDAR mounted on device left.
#define LeftRangeSensor       3  // Monitor Pin for LIDAR mounted on device left.
#define RightRangeTrigger     4   // Trigger Pin for LIDAR mounted on device right.
#define RightRangeSensor      6   // Monitor Pin for LIDAR mounted on device right.
#define FrontCollisionSensor  A0  // Front SharpIR sensor
#define BackCollisionSensor   A1  // Back SharpIR sensor
#define ServoPin              5   // Servo to control wheel turn (PWM)
#define EscPin                9   // Speed control (PWM)

Servo myservo;
Servo esc;              // ESC can be controlled like a servo.
char stop_start;        // Store latest xbee command
int safety_check;       // Store latest collision sensor reading
int distance_from_obstacle_1 = 0; // distance from the wall 1
int distance_from_obstacle_2 = 0; // distance from the wall 2
int pos = 0;                      // Position of the servo (degress, [0, 180])
int min_distance_to_wall ;        // keep track of the min distance
double maxSpeedOffset = 70; // maximum speed magnitude, in servo 'degrees'
double currentSpeedOffset = ESC_STOP; // start with speed 0
double minSpeedOffset = 80; // minimum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int currentTurnDegree = 0; //start with wheels in neutral position

int duration;
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
    delay(50);
  }
}

void calibrate_myservo()
{
  Serial.println("<===waiting for Xbee signal to start===>");
  while ( Serial.available() == 0) // while there is No Xbee signal to start MTA:
  {
    esc.write(ESC_STOP);                  // reset the ESC to neutral (non-moving) value
    pos = analogRead(myservo.attach(ServoPin));  // reads the value of the potentiometer (value between 0 and 1023)
    pos = map(pos, 0, 1023, 0, 179);      // scale it to use it with the servo (value between 0 and 180)
    myservo.write(90);                    // sets the servo position according to the scaled value
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
  Serial.println(safety_check);
}

void setup()
{
  Serial.begin(9600);
  stop_start = '0'; //stop
  Serial.println("< Calibration Mode started wait for 3 sec >");
  irSetup();
  timerSetup();
  lidarSetup();
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
  //straighten out
  if (currentTurnDegree != 0)
  {
    if (currentTurnDegree < 0) currentTurnDegree++;
    else currentTurnDegree--;
    myservo.write(90 + currentTurnDegree);
  }
  //speed up
  if (currentSpeedOffset > maxSpeedOffset) {
    currentSpeedOffset--;
    esc.write(currentSpeedOffset);
  }
  delay(100);
  Serial.println("forward Mode");
}

void turn(String dir, int turnDegree)
{
  // reduce the speed
  if (currentSpeedOffset < minSpeedOffset) {
    currentSpeedOffset++;
    esc.write(currentSpeedOffset);
  }
  if (dir == "left")
  {
    myservo.write(90 + turnDegree); // update the servo
    Serial.println("leftTurn Mode");
  }
  if (dir == "right")
  {
    myservo.write(90 - turnDegree); // update the servo
    Serial.println("rightTurn Mode");
  }
  delay(duration);
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
  delay(1000);
}

void swerve()    // used if the car is too close to a wall and should turn away from the wall
{
  Serial.println("Swerving");
  track_wall();
  if (wheel == "left_wheel")
  {
    currentTurnDegree++;
    double rad = degToRad(abs(currentTurnDegree));
    double wheelOffset = sin(rad) * maxWheelOffset;
    Serial.println("close to left wall");
    turn("right", wheelOffset);   // turn right
    Serial.println("calling rightTurn() function");
  }
  else if (wheel == "right_wheel")
  {
    currentTurnDegree--;
    double rad = degToRad(abs(currentTurnDegree));
    double wheelOffset = sin(rad) * maxWheelOffset;
    Serial.println("close to right wall");
    turn("left", wheelOffset);      // turn left
    Serial.println("calling leftTurn() function");
  }
  delay(INTERVAL);                // Delay between readings.
}

void loop()
{
  //int turn_angle = 0;       // local counter for turn degree
  forward();                // car moves forward continuously
  while (stop_start == '1' && safety_check <= IR_THRESHOLD && min_distance_to_wall >= BOUNDARY) // if far enough from both walls, keep going
  {
    forward();
    track_wall();
  }
  while (stop_start == '1' && safety_check <= IR_THRESHOLD && min_distance_to_wall < BOUNDARY) // if too close to a wall, turn away from wall
  {
    swerve();
  }
  while (stop_start == '0')
  {
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped because of x-bee command");
    delay(200);
  }
  while (safety_check > IR_THRESHOLD)
  {
    esc.write(ESC_STOP);    // abrupt stop
    Serial.println("Stopped to avoid imminent collision");
    delay(200);
  }
}
