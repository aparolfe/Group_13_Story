//#include <SimpleZigBeeRadio.h>
//#include <SoftwareSerial.h>
//SimpleZigBeeRadio xbee = SimpleZigBeeRadio();

float temp;
char mode[5];
double input[5];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  //Serial.println(Serial.available());
 //Serial.isComplete();
// Serial.println(Serial.available());
  if(Serial.available()>0)
  {
    //int x = Serial.read();
    //Serial.println("========xxxxx=========");
    //Serial.println(x);
    //Serial.println("========yyyyy=========");
    //delay(1000);
    //delay(1000);
    Serial.readBytes( mode,5);
    
   // Serial.write(temp_2);
    //temp = (float)Serial.read();
   //Serial.write(Serial.read());
   //Serial.println(temp);
   
   
   
  }
    for(int i=0; i < 4; i++)
       {
         
         Serial.print(mode[i]);
       }
    Serial.println("========yyyyy=========");
   //}
       // for(int i=0; i < 4; i++)
       //{
         //Serial.println(input[i]);
       //}
   //delay(1000);
   //int localtemp = temp_calc(input); 
    
   //Serial.println(input);
    //Serial.println(input.toInt());
    
   // Serial.write(Serial.read()-100);
    //Serial.println(a);
    //delay(1000);
  }
  



//double temp_calc(int row_value) 
//{
  //if ((row_value - 100 <100) && (row_value - 100>0)){return temp-100;}
  //if ((row_value - 200 <100) && (row_value - 100>0)){return temp-200;}
  //if ((row_value - 300 <100) && (row_value - 100>0)){return temp-300;}
  //if ((row_value - 400 <100) && (row_value - 100>0)){return temp-400;}
//} 
