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
    Serial.println("SoundArt Project Client");

    // SD INIT
    if (!SD.begin(D2))
    {
        Serial.println(F("SD ERROR"));
        return;
    }

    wifiManager.Connect();

    udp.begin(commPort);
    server.begin(commPort);
    device.SetAudio("/g4.mp3");
    device.SetDistance(30);
    device.SetVolume(20);
}

void loop()
{
     
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

    if (udp.parsePacket())
    {
        String s = udp.readStringUntil(':');
        if (s == "SoundArtCheckUp")
        {
            commPort = udp.readString().toInt();
            //Serial.println("port received: " + String(commPort));
            //Serial.println("IP: " + udp.remoteIP().toString() +
            //               " port: " + String(commPort));
            udp.beginPacket(udp.remoteIP().toString().c_str(), commPort);

            String message = "soundArtClientReply";
            Serial.println("sending:" + message);
            udp.print(message);
            udp.endPacket();
            //Serial.println(udp.getWriteError());r
            while (udp.available())
                udp.read();
            udp.stop();
            udp.begin(commPort);
        }
    }
    WiFiClient client = server.available();
    while (client.connected())
    {
        client.setNoDelay(true);
        if (client.available())
        {
            String cmd;
            String arg;

            while (client.available())
                cmd += static_cast<char>(client.read());
            Serial.println(cmd);
            arg = cmd.substring(cmd.indexOf(":") + 1);
            
            if (cmd.indexOf("setVolume") >= 0)
                device.SetVolume(arg.toInt());
            else if (cmd.indexOf("setLoop") >= 0)
                device.SetLoop(arg.toInt());
            else if (cmd.indexOf("setDistance") >= 0)
                device.SetDistance(arg.toInt());
            else if (cmd.indexOf("file") >= 0)
                ReceiveAudio(client, arg);
            else if (cmd.indexOf("test") >= 0)
                Serial.println("test:" + String(device.Test()));
            else if (cmd.indexOf("deleteAudio") >= 0)
                device.DeleteAudio(arg);

            client.stop();
        }
    }
    device.Run();
}
