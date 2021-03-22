#include <global.h>

#define NO_CONTENT_204 204
#define THIS_DEVICE 0

ESP8266WebServer server(80);
MWiFiManager wifiManager("/ap.cred");
String webpage = "/index.html";
std::vector<Device *> devices;
PhysicalDevice device;

bool EditFileLine(String filename, String content, String newContent)
{
    File file = SD.open(filename, "r");
    File tmp = SD.open(filenmame + ".tmp", "w");
    if (!file)
        Serial.println("can't open file for editing");
    String s = "";
    while (s.indexOf(content) == -1)
    {
        tmp.print(s);
        tmp.flush(); //
        if (!file.available())
        {
            Serial.println("file finished");
            return 0;
        }
        s = "";
        char c = file.peek();
        while (c != '\n' && file.available())
        {
            c = file.read();
            if (c != '\r')
                s += c;
            c = file.peek();
        }
        file.read();
        s += "\n";
    }
    //Serial.println("found place");
    tmp.println(newContent);

    s = file.readString();
    tmp.print(s);
    Serial.println("remaining: " + s);
    tmp.close();
    file.close();

    SD.remove(filename);

    file = SD.open(filename, "w");
    tmp = SD.open(filename + ".tmp", "r");
    while (tmp.available())
    {
        s = "";
        char c = tmp.peek();
        while (c != '\n' && tmp.available())
        {
            c = tmp.read();
            if (c != '\r')
                s += c;
            c = tmp.peek();
        }
        tmp.read();
        s += "\n";

        file.print(s);

        file.flush();
    }
    tmp.close();
    file.close();
    Serial.println("file modified");
    return 1;
}

void setup()
{
    //INITIALIZER
    Serial.begin(115200);
    Serial.println("");
    Serial.println("SoundArt Project");

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
        std::vector<String> nets = wifiManager.AvailableNetworks();
        /*Serial.print("networks available: ");
        for (auto i : nets)
            Serial.print(i + "   ---   ");
        Serial.println("");*/
        /*
        File file = SD.open("credentials.html", "r");
        File tmp = SD.open("credentials.tmp", "w");
        if (!file)
            Serial.println("can't open credential file");
        String s = "";
        while (s.indexOf("var wifis") == -1)
        {
            tmp.print(s);
            tmp.flush(); //
            if (!file.available())
            {
                Serial.println("file finished");
                exit(EXIT_FAILURE);
            }
            s = "";
            char c = file.peek();
            while (c != '\n' && file.available())
            {
                c = file.read();
                if (c != '\r')
                    s += c;
                c = file.peek();
            }
            file.read();
            s += "\n";
        }
        Serial.println("found place to put wifis");

        tmp.print("var wifis = [");
        for (size_t i = 0; i < nets.size() - 1; i++)
            tmp.print('\"' + nets[i] + '\"' + String(", "));
        tmp.println('\"' + nets.back() + "\"];");

        s = file.readString();
        tmp.print(s);
        Serial.println("remaining: " + s);
        tmp.close();
        file.close();

        SD.remove("credentials.html");

        file = SD.open("credentials.html", "w");
        tmp = SD.open("credentials.tmp", "r");
        while (tmp.available())
        {
            s = "";
            char c = tmp.peek();
            while (c != '\n' && tmp.available())
            {
                c = tmp.read();
                if (c != '\r')
                    s += c;
                c = tmp.peek();
            }
            tmp.read();
            s += "\n";

            file.print(s);

            file.flush();
        }
        tmp.close();
        file.close();
        Serial.println("file modified");
        */
       
        String newLine = "var wifis = [";
        for (size_t i = 0; i < nets.size() - 1; i++)
            newLine += '\"' + nets[i] + '\"' + String(", ");
        newLine += '\"' + nets.back() + "\"];";
        EditFileLine("credentials.html", "var wifis", newLine);

        wifiManager.SetAPMode(IPAddress(1, 2, 3, 4));
        webpage = "credentials.html";
    }

    if (webpage == "index.html")
        Serial.println("connected immediately");

    if (!MDNS.begin("esp8266"))
    { // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");

    // checkout for clients
    uint16_t udpPort = 8000;
    /*WiFiUDP udpServer;
    udpServer.begin(udpServer);
    String message = "SoundArtCheckUp:" +
                     WiFi.localIP().toString() +
                     ":" +
                     String(udpPort);
    SendBroadcastMessage(message, udpPort);
    unsigned long time = millis();*/
    std::vector<IPAddress> clients;
    /*while ((millis() - time < 10000))
    {
        if (udpServer.parsePacket() && udpServer.available())
        {
            if (udpServer.readStringUntil('\n') == "soundArtClientReply")
                if (std::find(clients.begin(), clients.end(), udpServer.remoteIP()) == clients.end())
                    clients.push_back(udpServer.remoteIP());

            while(udpServer.available()) udpServer.read();
        }
    }*/
    devices.resize(clients.size() + 1, nullptr);
    devices[0] = &device;

    for (size_t i = 0; i < clients.size(); i++)
        devices[i] = new RemoteDevice(clients[i], udpPort);

    // handlers linking

    server.on("/deleteAudio", HTTP_POST, handleDeleteAudio);
    server.on("/testDevice", HTTP_POST, handleTestDevice);
    server.on("/setVolume", HTTP_POST, handleSetVolume);
    server.on("/setAudio", HTTP_POST, handleSetAudio);
    server.on("/setLoop", HTTP_POST, handleSetLoop);
    server.on("/connectTo", HTTP_POST, handleConnectTo);
    server.on("/ipreq", HTTP_GET, [] {
        String msg = "connected to: " + WiFi.SSID() + " on ip: " +
                     WiFi.localIP().toString();
        server.send(200, "text/plain", msg);
    });
    server.on("/setNewWifi", HTTP_POST, handleSetNewWifi);
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
    device.Run();
}
