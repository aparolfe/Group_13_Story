#include <math.h>

const int PULLUP_RES = 90000; // in Ohm( 20kOm )

const double BETA = 4390; // in K for Semitec 104 GTA-2

const double THERMISTOR_RES = 100000; // in Ohm

const double THERMISTOR_NOM_TEMP = 23; // Celsius, C

void setup()
{
}

void loop()
{
 thermister_temp(analogRead(4));
 delay(2000);
}

void thermister_temp(int aval)
{
 double R, T;

R = (double) PULLUP_RES / ( (4095 / (double) aval ) - 1 );
 
 T = 1 / ( ( 1 / (THERMISTOR_NOM_TEMP + 273.15 )) + ( ( 1 / BETA) * log ( R / THERMISTOR_RES ) ) );
 
 T -= 273.15; // converting to C from K
 
 // return degrees C
// Spark.publish("Temperature", String(T) + " °C");
Particle.publish("temp4", temp4, PRIVATE);
}


