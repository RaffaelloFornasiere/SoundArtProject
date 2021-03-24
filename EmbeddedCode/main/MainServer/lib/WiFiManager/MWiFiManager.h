#ifndef MWIFI_MANAGER_H
#define MWIFI_MANAGER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "FS.h"
#include <Stream.h>
#include <string>
#include <vector>
#include <algorithm>
#include <SD.h>
#include <tuple>


enum class WiFiComp{Equals, SameSSID, Different};

class APCred
{
public:
    friend bool operator==(const APCred &ap1, const APCred &ap2) { return (ap1.psw == ap2.psw) && (ap1.ssid == ap2.ssid); }
    friend bool operator!=(const APCred &ap1, const APCred &ap2) { return !(ap1 == ap2); }

    APCred &operator=(const APCred &ap1)
    {
        ssid = ap1.ssid; psw = ap1.psw;
        return *this;
    }

    APCred(String ssid = "", String psw = "") : ssid(ssid), psw(psw) {}
    String toString() const { return String(ssid + " - " + psw); }

    void FromStream(Stream &s)
    {
        ssid = "";
        while (ssid == "")
        {
            ssid = s.readStringUntil('\n');
            ssid.trim();
        }
        psw = "";
        while (psw == "")
        {
            psw = s.readStringUntil('\n');
            psw.trim();
        }
    }

    String ssid;
    String psw;
};

class MWiFiManager
{
public:
    MWiFiManager(String filename) : filename(filename) { WiFi.mode(WIFI_STA); }
    bool Connect();
    void SetNewWiFi(const String &ssid, const String &psw);
    bool SetFileName(String filename)
    {
        this->filename = filename;
        return filename.endsWith(".cred");
    }
    bool Connected() { return (connected = (wifiMulti.run() == WL_CONNECTED)); }
    bool TryConnect(const String &ssid, const String &psw);
    bool TryConnect(APCred &ap) { return TryConnect(ap.ssid, ap.psw); }

    void SetAPMode();
    void SetAPMode(IPAddress ip);
    void SetSTAMode(IPAddress ip);
    void ClearAll();

    // std::vector<APCred> AvailableAPs();
    // std::vector<String> AvailableNetworks();
    std::vector<std::pair<APCred, int>> ScanWifis();

private:
    void DeleteAPFromSD(const APCred &ap);
    std::vector<APCred> GetAPFromSD();
    WiFiComp SearchAPInSD(const APCred& ap);

    String filename;
    ESP8266WiFiMulti wifiMulti;
    const String _fileID = ".wififile";
    bool connected;
};

#endif