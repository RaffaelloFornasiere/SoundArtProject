#ifndef MWIFI_MANAGER_H
#define MWIFI_MANAGER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "FS.h"
#include <Stream.h>
#include <string>
#include <vector>

#ifdef WIFIMANAGER_DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA__ARGS)
#define DEBUG_PRINTLN(...) Serial.println(__VA__ARGS)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

class MWiFiManager
{
public:
    MWiFiManager(String filename):filename(filename){}
    bool Connect();
    void SetNewWiFi(std::string ssid, std::string psw);
    void SetFileName(String filename) {this->filename = filename;}
    bool Connected() {return (connected = (wifiMulti.run() == WL_CONNECTED));}
    //void EnableAPMode() {}

private:
    class APCred
    {
    public:
        APCred(String ssid = "", String psw = "") : ssid(ssid), psw(psw) {}
        String toString() { return String(ssid + " - " + psw); }

        void FromStream(Stream &s)
        {
            ssid = "";
            while (ssid == "")
            {
                ssid = s.readStringUntil('\n');
                ssid.trim();
            }
            psw = "";
            while (ssid == "")
            {
                psw = s.readStringUntil('\n');
                psw.trim();
            }
            
        }

        String ssid;
        String psw;
    };
    String filename;
    ESP8266WiFiMulti wifiMulti; 
    const String _fileID = ".wififile";
    bool connected;
};

bool MWiFiManager::Connect()
{
    File file = SPIFFS.open(filename, "r");
    if (!file)
    {
        DEBUG_PRINTLN(F("WIFIMANAGER_ERROR_OPENING_FILE"));
        return 0;
    }

    if(file.available())
    {
        String fileID = file.readStringUntil('\n');
        if(fileID != _fileID)
        {
            DEBUG_PRINTLN(F("WIFIMANAGER_ERROR_FILE_ID"));
            return 0;
        }
    }

    std::vector<APCred> aps;
    while (file.available())
    {
        APCred aux;
        aux.FromStream(file); 
        wifiMulti.addAP(aux.ssid, aux.psw);
    }
    

    DEBUG_PRINTLN("Connecting ...");
    uint8_t secs = (millis() / 1000);
    connected = 0;
    while (!(connected = (wifiMulti.run() == WL_CONNECTED)))
    { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(1000);
        DEBUG_PRINTLN('.');
        if (millis() - secs * 1000 > 10000)
            break;
    }

    DEBUG_PRINTLN('\n');
    DEBUG_PRINTLN("Connected to ");
    DEBUG_PRINTLN(WiFi.SSID()); // Tell us what network we're connected to
    DEBUG_PRINTLN("IP address:\t");
    DEBUG_PRINTLN(WiFi.localIP());

    return connected;
}



#endif