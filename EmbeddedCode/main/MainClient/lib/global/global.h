#pragma once

#include <Arduino.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <FS.h>
#include <FTP.h>
#include <DNSServer.h>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <ESP8266WebServer.h>
#include <MWiFiManager.h>
#include <FileTransfer.h>
#include <Device.h>
#include <SD.h>

extern PhysicalDevice device;
extern MWiFiManager wifiManager;
extern uint16_t commPort;
extern WiFiServer server;
extern FileTransferManager ftm;



bool ReceiveAudio(WiFiClient& client, String fileName);