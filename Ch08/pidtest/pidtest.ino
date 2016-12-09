//set PID parameters
#include <PID_v1.h>
double Input_servo, Output_servo;
//double Input_speed, Output_speed;
double Setpoint = 90;
int Kp_servo = 2;
int Ki_servo = 0.05;
int Kd_servo = 0.5;
//int Kp_speed = 2;
//int Ki_speed = 0.05;
//int Kd_speed = 0.5;
PID myPID_servo(&Input_servo, &Output_servo, &Setpoint, Kp_servo, Ki_servo, Kd_servo, DIRECT);
//PID myPID_speed(&Input_speed, &Output_speed, &Setpoint, Kp_speed, Ki_speed, Kd_speed, DIRECT);
void setup() {
  // put your setup code here, to run once:
  myPID_servo.SetOutputLimits(-90, 90);
  myPID_servo.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:
  Input_servo = Serial.read();
  myPID_servo.Compute();
  Serial.println(Output_servo);
}
