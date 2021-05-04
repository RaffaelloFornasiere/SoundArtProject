#include <global.h>

ESP8266WebServer server(80);
MWiFiManager wifiManager("/ap.cred");
String webpage = "index.html";
std::vector<Device *> devices;
PhysicalDevice device;

const byte DNS_PORT = 53;
DNSServer dnsServer;

void setup()
{
    //INITIALIZER
    Serial.begin(500000);
    Serial.println("");
    Serial.println("SoundArt Project Server");

    /*    String reset = ESP.getResetInfo();
    Serial.println(reset);
    if(reset != "Software/System restart")
    {
        ESP.restart();
    }
*/

    //SPISettings sett(speed, MSBFIRST, SPI_MODE0);
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }
    Serial.println("SD initialized");

    //clock_t t = millis();

    //copyFile("/tone.mp3", "/tone.mp3.tmp");
    //Serial.println("file copied in: " + String(millis() - t));

    //WiFi connecting. Manages multiple APs and if if can't connect
    // to any of them generates a web page to configure wifi.
    // to reach the webpage you need to connect to ip 1.2.3.4
    if (!wifiManager.Connect())
    {
        Serial.println("non connected, ");
        webpage = "credentials.html";
    }

    wifiManager.SetAPMode();
    PrepareWebPage();
    setupHandlers();
    server.begin();
    Serial.println("Server started");

    device.SetDistance(30);
    device.SetVolume(0.02);
}

void loop()
{
    if (!WiFi.isConnected())
        Serial.println("eh ma dio cane");

    if (Serial.available())
    {
        Serial.println("ci sono");
        if (Serial.read() == 'r')
        {
            Serial.println("rebooting");
            ESP.restart();
            //ESP.reset();
        }

        while (Serial.available())
            Serial.read();
    }
    server.handleClient();
    device.Run();
}
