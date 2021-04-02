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
}

void loop()
{
    if (Serial.available())
    {
        if (Serial.read() == 'r')
            ESP.reset();
        while (Serial.available())
            Serial.read();
    }

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
    while (client.connected())
    {
        //client.setNoDelay(true);
        if (client.available())
        {
            String cmd = "";
            while (client.available())
                cmd += static_cast<char>(client.read());
            Serial.println(cmd);

            int fileE = 0;
            if (cmd.indexOf("setVolume") >= 0)
                device.SetVolume(cmd.substring(cmd.indexOf(":") + 1).toInt());
            else if (cmd.indexOf("setLopp") >= 0)
                device.SetLoop(cmd.substring(cmd.indexOf(":") + 1).toInt());
            else if (cmd.indexOf("file") >= 0)
            {
                String fileName = cmd.substring(cmd.indexOf(":") + 1);
                fileE = device.SetAudio("/" + fileName);
                Serial.println("fileE: " + String(fileE));
                if (fileE == 0)
                {
                    Serial.println("notFound");
                    client.print("notFound");
                    
                    if (client.connected())
                    {
                        File file = SD.open(fileName, "w");
                        if (!file)
                            Serial.println("can't open file for write");
                        ftm.ReadFrom(client, file);
                    }
                    else
                    {
                        Serial.println("error");
                    }

                }
                else
                    client.print("ok");
            }
            else if (cmd.indexOf("test") >= 0)
                device.Test();
        }
    }
    device.Run();
}
