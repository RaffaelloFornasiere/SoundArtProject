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
extern PhysicalDevice device;


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
void handleSetNewWifi();
void setupHandlers();




void SendBroadcastMessage(String message, uint16_t port);
bool EditFileLine(String filename, String content, String newContent);
bool EditFileLines(String filename, std::vector<String> content, std::vector<String> newContent);
bool copyFile(String src, String dst);
bool SetupDevices();
bool PrepareWebPage();
bool PrepareIndexPage2(int nOfDevices);
bool PrepareCredentialsPage();
std::vector<IPAddress> FindDevices(uint16_t);