#include <global.h>



ESP8266WebServer server(80);
MWiFiManager wifiManager("/ap.cred");
String webpage = "index.html";
std::vector<Device *> devices;
PhysicalDevice device;


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

    // SD INIT
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }
    Serial.println("SD initialized");

    //WiFi connecting. Manages multiple APs and if if can't connect
    // to any of them generates a web page to configure wifi.
    // to reach the webpage you need to connect to ip 1.2.3.4
    if (!wifiManager.Connect())
    {
        Serial.println("non connected, ");
        webpage = "credentials.html";
    }

    PrepareWebPage();
    setupHandlers();
    server.begin();
    Serial.println("Server started");
}

void loop()
{
    if(Serial.available())
    {
        if(Serial.read() == 'r');
            ESP.reset();
        while(Serial.available()) Serial.read();
    }
    server.handleClient();
    device.Run();
}
