#ifndef MWIFI_MANAGER_H
#define MWIFI_MANAGER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "FS.h"
#include <Stream.h>
#include <string>
#include <vector>
#include <SD.h>

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
    MWiFiManager(String filename) : filename(filename) {}
    bool Connect();
    void SetNewWiFi(const String& ssid, const String& psw);
    void SetFileName(String filename) { this->filename = filename; }
    bool Connected() { return (connected = (wifiMulti.run() == WL_CONNECTED)); }
    void SetAPMode();
    void SetAPMode(IPAddress ip);
    void ClearAll();

    std::vector<String> AvailableNetworks();

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

#endif