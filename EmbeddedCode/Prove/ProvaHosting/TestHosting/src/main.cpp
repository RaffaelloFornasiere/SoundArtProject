#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include "FS.h"
#include <MVector.h>
#include <FTP.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <string>

ESP8266WiFiMulti wifiMulti; 
//WiFiManager wifiManager;


MVector<String> WifiAPs()
{
    MVector<String> res;
    uint8_t n = WiFi.scanNetworks();
    for(uint8_t i = 0; i < n; i++)
      res.push_back(WiFi.SSID(1));
    
    return res;
}



void RunWiFiSetup()
{

}

void WiFiConnect()
{

  // opens file with credentials of 
  File file = SPIFFS.open(F("/apcredentials/ap.txt"), "r");
  if(!file){
    Serial.println(F("FILE ERROR"));
    return;
  }

  class APCred
  {
  public:
    
    APCred(String ssid = "", String psw = "") : ssid(ssid), psw(psw) {}
    String toString() {return String(ssid + " - " + psw);}

    String ssid;
    String psw;
  };

  MVector<APCred> aps;
  
  while(file.available())
  {
    APCred aux; 
    while( aux.ssid == "")
    {
      aux.ssid = file.readStringUntil('\n');
      aux.ssid.trim();
    }
    aux.psw = "";
    while(aux.psw == "")
    {
      aux.psw = file.readStringUntil('\n');
      aux.psw.trim();
    }
    aps.push_back(aux);
  }
  //Serial.println(aps.size());
  for(auto a : aps)
  {
    char c[a.ssid.length()]; a.ssid.toCharArray(c, a.ssid.length()+1);
    char p[a.psw.length()]; a.psw.toCharArray(p, a.psw.length()+1); 
    //Serial.println(String(c) + " " + String(p));
    wifiMulti.addAP(c,p);
  }
  
  

  Serial.println("Connecting ...");
  uint8_t secs = millis()/1000;
  bool connected = 0;
  while (!(connected = (wifiMulti.run() == WL_CONNECTED))) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(500);
    Serial.print('.');
    if(millis() - secs*1000 > 10000)
      break;
  }
  if(!connected)
    RunWiFiSetup();


  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  
  if(!SPIFFS.begin()){
    Serial.println(F("SPIFF ERROR"));
    return;
  }
  WiFiConnect();
}
 
void loop() 
{

}

