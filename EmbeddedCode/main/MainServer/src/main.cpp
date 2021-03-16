#include <global.h>

#define NO_CONTENT_204 204
#define THIS_DEVICE 0

ESP8266WebServer server(80);
MWiFiManager wifiManager("/ap.txt");
String webpage = "/index.html";
std::vector<Device *> devices;




void setup()
{
    //INITIALIZER
    Serial.begin(115200);
    Serial.println("");
    Serial.println("SoundArt Project");



    if (!SPIFFS.begin())
    {
        Serial.println(F("SPIFFS ERROR"));
        return;
    }

    // SD INIT
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }

    if (!wifiManager.Connect())
    {
        std::vector<String> nets = wifiManager.AvailableNetworks();
        File file = SPIFFS.open("credentials.html", "r+");
        String s;
        do
        {
            s = file.readStringUntil('\n');
            s.trim();
        } while (s != "//addWifiNames");

        //prints the available networks in the html file
        file.print("let wifis = [");
        for (auto i : nets)
            file.print(i + String(", "));
        file.print("]; \n");

        wifiManager.SetAPMode(IPAddress(1, 2, 3, 4));
        webpage = "credentials.html";
    }

    // checkout for clients
    uint16_t udpPort = 8000;
    WiFiUDP udpServer;
    udpServer.begin(udpServer);
    String message = "SoundArtCheckUp:" +
                     WiFi.localIP().toString() +
                     ":" +
                     String(udpPort);
    SendBroadcastMessage(message, udpPort);
    unsigned long time = millis();
    std::vector<IPAddress> clients;
    while ((millis() - time < 10000))
    {
        if (udpServer.parsePacket() && udpServer.available())
        {
            if (udpServer.readStringUntil('\n') == "soundArtClientReply")
                if (std::find(clients.begin(), clients.end(), udpServer.remoteIP()) == clients.end())
                    clients.push_back(udpServer.remoteIP());

            while(udpServer.available()) udpServer.read();
        }
    }
    devices.resize(clients.size() +1, nullptr);
    devices[0] = new PhysicalDevice();
    for(size_t i = 0; i < clients.size(); i++)
        devices[i] = new RemoteDevice(clients[i], udpPort);

    


    // handlers linking
    server.on("/deleteAudio", HTTP_POST, handleDeleteAudio);
    server.on("/testDevice", HTTP_POST, handleTestDevice);
    server.on("/setVolume", HTTP_POST, handleSetVolume);
    server.on("/setAudio", HTTP_POST, handleSetAudio);
    server.on("/setLoop", HTTP_POST, handleSetLoop);
    server.on("/connectTo", HTTP_POST, handleConnectTo);
    server.on("/clearWifiSettings", HTTP_POST, hanldeClearWifiSettings);
    server.on(
        "/upload/uri", HTTP_POST, // if the client posts to the upload page
        []() { server.send(200); },
        handleFileUpload // Receive and save the file
    );
    server.onNotFound([&]() {
        // If the client requests any file
        // send it if it exists
        // otherwise, respond with a 404 (Not Found) error
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "404: Not Found");
    });
    server.begin();
}

void loop()
{
    server.handleClient();
}
