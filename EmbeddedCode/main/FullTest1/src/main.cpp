#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include "../include/Vector/MVector.h"
#include <FTP.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include "../include/WiFiManager/MWiFiManager.h"
#include <ESP8266WebServer.h>
#include "../include/MWebServer/MWebServer.h"


#define NO_CONTENT_204 204



/*std::vector<String> WifiAPs()
{
    std::vector<String> res;
    uint8_t n = WiFi.scanNetworks();
    for (uint8_t i = 0; i < n; i++)
        res.push_back(WiFi.SSID(1));

    return res;
}*/

String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
WebServer server("index.html");

String webpage = "index.html";
void setup()
{
    Serial.begin(115200);

    if (!SPIFFS.begin())
    {
        Serial.println(F("SPIFF ERROR"));
        return;
    }

    //
    MWiFiManager wifiManager("./wifimanager/ap.txt");
    if (!wifiManager.Connect())
    {
        WiFi.mode(WIFI_AP); //need both to serve the webpage and take commands via tcp
        IPAddress ip(1, 2, 3, 4);
        IPAddress gateway(1, 2, 3, 1);
        IPAddress subnet(255, 255, 255, 0);
        WiFi.softAPConfig(ip, gateway, subnet);
        WiFi.softAP("SoundArtAP"); //Access point is open
        delay(1000);
        webpage = "wifisettings.html";
    }

    /*server.onNotFound([]() {                                  // If the client requests any URI
        if (!handleFileRead(server.uri()))                    // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();*/
}

void loop()
{
    //server.handleClient();
}
