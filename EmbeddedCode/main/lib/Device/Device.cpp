#include "Device.h"

FileTransferManager RemoteDevice::ftm(FileTransferManager::FileTransferPort);

bool PhysicalDevice::SetVolume(int volume)
{
    double divider = 400.0;
    Serial.println("PhysicalDevice::SetVolume: " + String(volume / divider));
    outI2s.SetGain(volume / divider);
    return true;
}

bool PhysicalDevice::SetAudio(String audio)
{
    Serial.println("PhysicalDevice::SetAudio");
    if (mp3.isRunning())
        mp3.stop();
    if (sdFile.isOpen())
        sdFile.close();
    fileName = audio;
    
    if (sdFile.open(fileName.c_str()))
        mp3.begin(&sdFile, &outI2s);

    return sdFile.isOpen();
}

bool PhysicalDevice::Test()
{
    Serial.println("PhysicalDevice::Test");
    if (mp3.isRunning())
        mp3.stop();
    if (sdFile.isOpen())
        sdFile.close();
    if (!sdFile.open("/test/test.mp3"))
        return false;
    mp3.begin(&sdFile, &outI2s);
    while (mp3.isRunning())
    {
        if (!mp3.loop())
            mp3.stop();
    }

    sdFile.close();
    sdFile.open(fileName.c_str());
    mp3.begin(&sdFile, &outI2s);
    return true;
}

bool PhysicalDevice::clearAudios()
{
    Serial.println("PhysicalDevice::clearAudios");
    File root = SD.open("/");
    File file = file.openNextFile();
    while (file)
    {
        while (file.isDirectory())
        {
            file.close();
            file = root.openNextFile();
        }

        String fName = file.name();
        file.close();
        file = root.openNextFile();
        if (fName.endsWith(".mp3"))
            SD.remove(fName);
    }
    return true;
}

bool PhysicalDevice::SetLoop(bool loop)
{
    Serial.println("PhysicalDevice::setLopp");
    this->loop = loop;
    return true;
}

void PhysicalDevice::Run()
{
    //Serial.println("PhysicalDevice::Run");
    if (hc.isFinished())
    {
        //Serial.println("get dist");
        double dist = hc.getRange();
        if (dist < 20)
        {
            if (mp3.isRunning())
            {
                if (!mp3.loop())
                {
                    mp3.stop();

                    if (loop)
                    {
                        sdFile.close();
                        sdFile.open(fileName.c_str());
                        if (mp3.begin(&sdFile, &outI2s))
                            Serial.println("all ok");
                    }
                }
            }
        }
        hc.start();
    }
}

bool RemoteDevice::SetVolume(int volume)
{
    Serial.println("RemoteDevice::SetVolume");
    return _send("setVolume:" + String(volume));
}

bool RemoteDevice::SetAudio(String audio)
{
    Serial.println("RemoteDevice::SetAudio");
    client.connect(ip, port);
    bool res;
    if (client.connected())
    {
        client.print("file:" + audio);
        // wait for checking
        delay(1000);
        String resp = client.readString();
        if (resp == "notFound")
        {
            Serial.println("sending file");
            File file = SD.open(audio, "r");
            if(!file)
            {
                Serial.println("can't open file to send");
                return false;
            }
            ftm.Send(ip, FileTransferManager::FileTransferPort, file);
            client.stop();
            SetAudio(audio);
        }
        res = true;
    }
    res = false;
    client.stop();
    return res;
}

bool RemoteDevice::SetLoop(bool loop)
{
    Serial.println("RemoteDevice::SetLoop");
    return _send("setLoop:" + String(loop));
}

bool RemoteDevice::Test()
{
    Serial.println("RemoteDevice::SetATestudio");
    return _send("test");
}

bool RemoteDevice::clearAudios()
{
    Serial.println("RemoteDevice::clearAudios");
    return _send("clearAudios");
}