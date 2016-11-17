// Pins Used
const int button = 8;
const int red_LED = 4;
const int green_LED = 5;
const int blue_LED = 6;
       
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

volatile int is_infected = 0;
volatile int is_cleared = 0;
int is_leader = 1;

void setup() {
  // LEDs:
  Serial.begin(9600);
  pinMode(red_LED, OUTPUT); // Red
  pinMode(green_LED, OUTPUT); // Green
  pinMode(blue_LED, OUTPUT); // Blue    
  digitalWrite(red_LED, LOW);
  digitalWrite(green_LED, LOW);
  digitalWrite(blue_LED, LOW);
  // Button :
  pinMode(button,INPUT);
 
}

void react() {
  Serial.println("in react");
  if (is_leader == 0) is_infected = 1;
  else is_cleared = 1;
}

void loop() {
    
    if (is_infected == 1) digitalWrite(red_LED, HIGH);
    if (is_cleared == 1) digitalWrite(green_LED, HIGH);
    if (is_leader == 1) digitalWrite(blue_LED, HIGH);

    //debouncing
  int reading = digitalRead(button);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        react();
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  Serial.println(lastButtonState);
  
  
}
