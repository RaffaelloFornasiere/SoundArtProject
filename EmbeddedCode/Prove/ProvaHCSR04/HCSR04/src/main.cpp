#include <Arduino.h>
#include <HCSR04.h>

HCSR04 hc(D1, D2);

void setup()
{ 
  Serial.begin(115200); 
}

void loop() 
{ 
  Serial.println( hc.dist() ); 
}