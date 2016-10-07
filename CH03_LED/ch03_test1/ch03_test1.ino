int val5 = 0;
int val6 = 0;
int val7 = 0;
int val8 = 0;
int val13 = 0;
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
    Serial.print(digitalRead(5));
    Serial.print(digitalRead(6));
    Serial.print(digitalRead(7));
    Serial.print(digitalRead(8));
    Serial.print(digitalRead(13));
    Serial.println(",");  

if( Serial.available()>0)
{
  //Serial.println("================= ");
    
    //Serial.println("================= ");
    String first  = Serial.readStringUntil(',');
    int LED_Nu = first.toInt();
    //Serial.read(); //next character is comma, so skip it using this
    int second = Serial.readStringUntil('.').toInt();
    //Serial.println(LED_Nu);
    //Serial.println(second);
  switch (LED_Nu) {
    
     case 5:   
      if (second == 0)
       {digitalWrite(5, LOW);
       //Serial.println("the value is 0 of port 5");
       break;
       }else if (second ==1)
       digitalWrite(5, HIGH);
       //Serial.println("the value is 1 of port 5");
     break;
      
    case  6: 
     if (second == 0)
       {digitalWrite(6, LOW);
       //Serial.println("the value is 0 of port 6");
       break;}
       
       else if (second ==1)
       digitalWrite(6, HIGH);
       //Serial.println("the value is 1 of port 6");
       break;
    
    case  7: 
        if (second == 0)
       {digitalWrite(7, LOW);
       //Serial.println("the value is 0 of port 7");
       break;
       }else if (second ==1)
       digitalWrite(7, HIGH);
       //Serial.println("the value is 1 of port 7");
       break;
    
    case  8: 
        if (second == 0)
       {digitalWrite(8, LOW);
       //Serial.println("the value is 0 of port 8");
       break;}
       else if (second ==1)
       digitalWrite(8, HIGH);
       //Serial.println("the value is 1 of port 8");
       break;

     case  13: 
        if (second == 0)
       {digitalWrite(13, LOW);
       //Serial.println("the value is 0 of port 13");
       
       break;}
       else if (second ==1)
       digitalWrite(13, HIGH);
       //Serial.println("the value is 1 of port 13");
       break;
  }
   
} 
delay(500);      
}
