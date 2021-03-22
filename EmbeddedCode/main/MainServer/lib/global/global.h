#pragma once

#include <Arduino.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <FTP.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <ESP8266WebServer.h>
#include <MWiFiManager.h>
#include <Device.h>
#include <SD.h>


extern ESP8266WebServer server;
extern MWiFiManager wifiManager;
extern String webpage;
extern std::vector<Device *> devices;

String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleSetVolume();
void handleDeleteAudio();
void handleTestDevice();
void handleSetAudio();
void handleSetLoop();
void handleConnectTo();
void hanldeClearWifiSettings();
void SendBroadcastMessage(String message, int port);
void handleSetNewWifi();