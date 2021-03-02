
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com  
*********/

#include "FS.h"



void setup() {

  Serial.begin(115200);
 
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  Dir dir = SPIFFS.openDir ("");
  while (dir.next ()) {
    Serial.println (dir.fileName ());
    Serial.println (dir.fileSize ());
  }
  
}

 

 
void loop() {}
