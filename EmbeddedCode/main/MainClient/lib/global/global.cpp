#include "global.h"

bool ReceiveAudio(WiFiClient& client, String fileName)
{
    int fileE = device.SetAudio("/" + fileName);
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
        
    fileE = device.SetAudio("/" + fileName);
    return fileE;
}