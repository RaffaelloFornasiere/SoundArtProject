#include <global.h>

#define NO_CONTENT_204 204
#define THIS_DEVICE 0

MWiFiManager wifiManager("/ap.cred");

uint16_t commPort = 8000;
WiFiServer server(commPort);
FileTransferManager ftm(FileTransferManager::FileTransferPort);
PhysicalDevice device;
WiFiUDP udp;
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

    udp.begin(commPort);
    server.begin(commPort);
}

void loop()
{
    if (udp.parsePacket())
    {
        String s = udp.readStringUntil(':');
        if (s == "SoundArtCheckUp")
        {
            commPort = udp.readString().toInt();
            Serial.println("port received: " + String(commPort));
            Serial.println("IP: " + udp.remoteIP().toString() +
                           " port: " + String(commPort));
            udp.beginPacket(udp.remoteIP().toString().c_str(), commPort);

            String message = "soundArtClientReply";
            Serial.println("sending:" + message);
            udp.print(message);
            udp.endPacket();
            Serial.println(udp.getWriteError());
            while (udp.available())
                udp.read();
            udp.stop();
            udp.begin(commPort);
        }
    }
    WiFiClient client = server.available();
    if (client)
    {
        Serial.println("new cmd");
        String cmd = client.readStringUntil(':');
        Serial.println(cmd);
        int fileE = 0;
        if (cmd == "setVolume")
            device.SetVolume(client.readString().toInt());
        else if (cmd == "setLoop")
            device.SetLoop(client.readString().toInt());
        else if (cmd == "file")
        {
            fileE = device.SetAudio("/" + client.readString());
            if (fileE == 0)
            {
                client.print("notFound");
                File file = SD.open(client.readString(), "w");
                ftm.Read(file);
            }
        }
        else if (cmd == "test")
            device.Test();

        client.stop();
    }
    device.Run();
}
