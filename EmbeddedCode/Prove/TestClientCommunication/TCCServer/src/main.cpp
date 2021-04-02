
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <vector>
#include <algorithm>
#include <SD.h>
#include "../lib/FileTransfer/FileTransfer.h"

int port = 8888; //Port number
uint16_t udpPort = 8887;
ESP8266WebServer server(80);
WiFiUDP udp;

//Server connect to WiFi Network
const char *ssid = "AndroidS6";
const char *password = "q2345678"; //Enter your wifi Password

void SendBroadcastMessage(String message, int port)
{
    WiFiUDP udp; //udp.begin(port);
    udp.beginPacket("192.168.0.255", port);
    Serial.println("Sending: " + message);
    size_t payload = udp.write(message.c_str());
    udp.endPacket();
    Serial.println("bytes sent: " + String(payload));
}

//=======================================================================
//                    Power on setup
//=======================================================================

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)
void handleFileUpload();                // upload a new file to the SPIFFS

void setup()
{
    Serial.begin(500000);
    //pinMode(SendKey, INPUT_PULLUP); //Btn to send data
    Serial.println();
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }

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

    Serial.print("Server IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print(" port ");
    Serial.println(port);
    udp.begin(port);

    bool on = false;
}
//=======================================================================
//                    Loop
//=======================================================================

std::vector<IPAddress> devices;

FileTransferManager ftm(8000);
void loop()
{
    /*
    // send broadcast to find clients
    if (Serial.available() && char(Serial.read()) == 'c')
    {
        while (Serial.available())
            Serial.read();
        String message = "SoundArtCheckUp:" +
                         WiFi.localIP().toString() +
                         ":" +
                         String(port);
        SendBroadcastMessage(message, port);
    }

    //parse clients
    if (udp.parsePacket())
    {
        while (udp.available())
        {
            //Serial.println("new client:" + udp.remoteIP().toString());

            if (std::find(devices.begin(), devices.end(), udp.remoteIP()) == devices.end())
            {
                devices.push_back(udp.remoteIP());
                //Serial.println("/t device added");
            }
            while (udp.available())
                udp.read();

            //Serial.println(udp.readString());
        }
    }*/
    /*if (on == false)
    {
        File f = SPIFFS.open("/Test1.html", "r");
        while (f.available())
        {
            Serial.println("available: " + String(f.available()));
            char c[std::min(1000 + 1, f.available() +1)];

            f.readBytes(c, 100);
        }
        on = true;
    }*/

    if (Serial.available())
    {
        while (Serial.available())
            Serial.read();

       

            File f = SD.open("/tone.mp3", "r");
            Serial.println("begin: ");
            long unsigned time = millis();
            ftm.Send("192.168.43.100", 8000, f);
            Serial.println("Sent in: " + String(millis() - time) + " ms");

    }
}
