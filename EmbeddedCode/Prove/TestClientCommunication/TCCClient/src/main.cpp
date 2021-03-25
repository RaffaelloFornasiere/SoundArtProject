
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "../lib/FileTransfer/FileTransfer.h"
#include <SPI.h>
#include <SD.h>
#include <fstream>
#include <iostream>
#include <string>

#define SendKey 0 //Button to send data Flash BTN on NodeMCU

int port = 8000; //Port number
int udpPort = 8888;
WiFiServer server(port);
WiFiUDP udp;
HTTPUpload up;

//Server connect to WiFi Network
const char *ssid = "FASTWEB-NGCN3V";
const char *password = "G3P32AOH7L"; //Enter your wifi Password

int count = 0;
//=======================================================================
//                    Power on setup
//=======================================================================
void setup()
{

    Serial.begin(500000);
    pinMode(SendKey, INPUT_PULLUP); //Btn to send data
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); //Connect to wifi

    // Wait for connection
    Serial.println("Connecting to Wifi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);

    Serial.print("Client IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print(" port ");
    Serial.println(port);

    Serial.print("Initializing SD card...");

    if (!SD.begin(D2))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    server.begin();

    udp.begin(udpPort);
}
//=======================================================================
//                    Loop
//=======================================================================

FileTransferManager ftm(8000);
WiFiClient client;
bool received = 0;
void loop()
{
    //WiFiClient client; /*= server.available();
    /*if (udp.parsePacket())
    {
        String s = udp.readStringUntil(':');
        if (s == "SoundArtCheckUp")
        {
            IPAddress src;
            src.fromString(udp.readStringUntil(':'));
            uint16_t srcport = udp.readString().toInt();
            udp.beginPacket(src.toString().c_str(), srcport);

            String message = "client:" +
                             WiFi.localIP().toString();
            Serial.println("sending:" + message);
            udp.write(message.c_str());
            udp.endPacket();
            Serial.println(udp.getWriteError());
            while (udp.available())
                udp.read();
        }
    }
*/
    if (!received)
    {
        File f = SD.open("/test.mp3", "w");
        if (!f)
        {
            Serial.println("can't open file");
            return;
        }
        else
        {
            
            received = ftm.Read(f);
        }
        f.close();

        if (received)
        {
            f = SD.open("/test.mp3");
            if (!f)
            {
                Serial.println("can't open file");
                return;
            }
            else
                Serial.println(f.size());
        }
        
    }
}