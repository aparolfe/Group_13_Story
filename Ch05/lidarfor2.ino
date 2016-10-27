unsigned long pulse_widthleft;
unsigned long pulse_widthright;
void setup()
{
  Serial.begin(9600); // Start serial communications
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  pinMode(4, OUTPUT); // Set pin 4 as trigger pin
  pinMode(6, INPUT); // Set pin 6 as monitor pin
  digitalWrite(4, LOW); // Set trigger LOW for continuous read
}

void loop()
{
  pulse_widthleft = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  if(pulse_widthleft != 0){ // If we get a reading that isn't zero, let's print it
     pulse_widthleft = pulse_widthleft/10; // 10usec = 1 cm of distance for LIDAR-Lite
  pulse_widthright = pulseIn(6, HIGH); // Count how long the pulse is high in microseconds
  if(pulse_widthright != 0){ // If we get a reading that isn't zero, let's print it
     pulse_widthright = pulse_widthright/10; // 10usec = 1 cm of distance for LIDAR-Lite
    Serial.println(pulse_widthleft); // Print the distance
    Serial.println(pulse_widthright); // Print the distance
  }
  delay(20); //Delay so we don't overload the serial port
}
}
