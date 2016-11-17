
const int del = 1000;

// Pins Used
const int button = 2;
const int red_LED = 4;
const int green_LED = 5;
const int blue_LED = 6;

volatile int is_infected = 0;
volatile int is_cleared = 0;
int is_leader = 0;

void setup() {
  // LEDs:
  pinMode(red_LED, OUTPUT); // Red
  pinMode(green_LED, OUTPUT); // Green
  pinMode(blue_LED, OUTPUT); // Blue    
  digitalWrite(red_LED, LOW);
  digitalWrite(green_LED, LOW);
  digitalWrite(blue_LED, LOW);
  // Button :
  pinMode(button,INPUT);
  attachInterrupt(digitalPinToInterrupt(button), react, RISING);
}

void react() {
  if (is_leader == 0) is_infected = 1;
  else is_cleared = 1;
}

void loop() {
    if (is_infected == 1) digitalWrite(red_LED, HIGH);
    if (is_cleared == 1) digitalWrite(green_LED, HIGH);
    if (is_leader == 1) digitalWrite(blue_LED, HIGH);
    delay(del);
}
