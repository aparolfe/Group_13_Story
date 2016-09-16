#include <math.h> 
#include <SoftwareSerial.h>
SoftwareSerial XBee(2,3);

double Temp;
 
void setup() 
{           
  Serial.begin(9600); 
 
}
 
double temp_calc(int anal_value) 
{
 Temp = log(((10240000/anal_value) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15; 
 return Temp;
}

void loop() 
{            
  int val;              
  val=analogRead(0);      
  Temp=temp_calc(val);   
  Serial.println(Temp); 
  //Serial.print(Temp);  
  delay(2000);            
 }
