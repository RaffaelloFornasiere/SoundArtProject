#include <global.h>

#define NO_CONTENT_204 204
#define THIS_DEVICE 0

MWiFiManager wifiManager("/ap.cred");

uint16_t commPort = 8000;
WiFiServer server(commPort);
FileTransferManager ftm(FileTransferManager::FileTransferPort);
PhysicalDevice device;

void setup()
{
    //INITIALIZER
    Serial.begin(500000);
    Serial.println("");
    Serial.println("SoundArt Project");

    // SD INIT
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }

    wifiManager.Connect();

    WiFiUDP udp;
    udp.begin(commPort);
    WiFiClient client;
    while (!udp.parsePacket())
        ;

    String s = udp.readStringUntil(':');
    if (s == "SoundArtCheckUp")
    {
        IPAddress src;
        src.fromString(udp.readStringUntil(':'));
        commPort = udp.readString().toInt();
        udp.beginPacket(src.toString().c_str(), commPort);

        String message = "client:" +
                         WiFi.localIP().toString();
        Serial.println("sending:" + message);
        udp.print(message);
        udp.endPacket();
        Serial.println(udp.getWriteError());
        while (udp.available())
            udp.read();
    }
    else
    {
        return;
    }
    server.begin(commPort);
}

void loop()              
{
    WiFiClient client = server.available();
    if (client)
    {
        String cmd = client.readStringUntil(':');
        int fileE = 0; 
        if (cmd == "setVolume")
            device.SetVolume(client.readString().toInt());
        else if (cmd == "setLoop")
            device.SetLoop(client.readString().toInt());
        else if (cmd == "file")
            fileE = device.SetAudio("/" + client.readString());
        else if (cmd == "test");
            device.Test();
        
        if(fileE == 0)
            client.print("notFound");
        client.stop();        
    }
    device.Run();
}
