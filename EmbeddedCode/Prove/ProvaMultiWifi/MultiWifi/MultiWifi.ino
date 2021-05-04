#include "ESP8266WiFi.h"

#include <ESP8266WiFi.h>        // Include the Wi-Fi library

const char* ssid     = "TP-Link_739F";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "32527476";     // The password of the Wi-Fi network


void setup()
{
  Serial.begin(115200);
  Serial.println("");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Scan start ... ");
  int n = WiFi.scanNetworks();
  Serial.print(n);
  Serial.println(" network(s) found");
  for (int i = 0; i < n; i++)
  {
    Serial.println(WiFi.SSID(i));
  }
  Serial.println();

  delay(1000);
   Serial.println();
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());

     
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void loop()
{
  
}
