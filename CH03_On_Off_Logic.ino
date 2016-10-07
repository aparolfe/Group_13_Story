
void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  

if( Serial.available()>0)
{
  char Value_Reading = Serial.read();
  switch (Value_Reading ) {
     case 'C':   
     digitalWrite(13, LOW);
     Serial.println("the value is 0");
     break;
      
    case 'O': 
    digitalWrite(13, HIGH);
    Serial.println("the value is 1");
    break;
  }
  delay(1000); 
}       
}





