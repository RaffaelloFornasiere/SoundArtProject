#ifndef DEVICE_H
#define DEVICE_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FileTransfer.h>
#include <HC_SR04.h>
#include <ESP8266HTTPClient.h>
#include <SD.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

// USES SD and suppose that is already initialized

class Device
{
public:
    Device() {}

    ~Device() {}
    virtual bool SetVolume(int volume) = 0;
    virtual bool SetLoop(bool on) = 0;
    virtual bool SetAudio(String audio) = 0;
    virtual bool Test() = 0;
};

class PhysicalDevice : public Device
{
public:
    PhysicalDevice(uint8_t trigPin = D3, uint8_t echoPin = D2)
        : hc(trigPin, echoPin), loop(false), fileName("test.mp3") { hc.start(); }
    bool SetVolume(int volume) override{outI2s.SetGain(volume/25.0); return true;}
    bool SetLoop(bool on) override { loop = true; return true;}
    bool SetAudio(String audio) override
    {
        if (mp3.isRunning())
            mp3.stop();
        if (sdFile.isOpen())
            sdFile.close();
        fileName = audio;
        sdFile.open(fileName.c_str());
        return sdFile.isOpen();
    };
    bool Test()
    {
        if (mp3.isRunning())
            mp3.stop();
        if (sdFile.isOpen())
            sdFile.close();
        sdFile.open("test.mp3");
        mp3.begin(&sdFile, &outI2s);
        while(mp3.isRunning())
        {
            if(!mp3.loop())
                mp3.stop();
        }
        sdFile.close();
        sdFile.open(fileName.c_str());
        mp3.begin(&sdFile, &outI2s);
        return true;
    }

    void Run()
    {
        if (hc.isFinished())
        {
            double dist = hc.getRange();
            if (dist < 20)
            {
                if (!mp3.loop())
                    mp3.stop();

                if (loop)
                {
                    sdFile.close();
                    sdFile.open("/tone.mp3");
                    if (mp3.begin(&sdFile, &outI2s))
                        Serial.println("all ok");
                }
            }
            hc.start();
        }
    }

private:
    HC_SR04 hc;
    bool loop;

    String fileName;

    AudioOutputI2S outI2s;
    AudioGeneratorMP3 mp3;
    AudioFileSourceSD sdFile;
};

class RemoteDevice : public Device
{
public:
    RemoteDevice(IPAddress ip, uint16_t port) : ip(ip), port(port) {}
    bool SetVolume(int volume) override
    {
        return _send("setVolume:" + String(volume));
    }
    bool SetLoop(bool on) override
    {
        return _send("setLoop:" + String(on));
    }
    bool SetAudio(String audio) override
    {
        WiFiClient client;
        client.connect(ip, port);
        client.connect(ip, port);
        //bool res = client.connected();
        if (client.connected())
        {
            client.print("file:" + audio);
            //delay(100)?
            String resp = client.readString();
            if (resp == "exist")
                return true;

            File file = SD.open(audio, "r");
            ftm.Send(ip, file);
            return true;
        }
        return false;
    }
    bool Test() override
    {
        return _send("test");
    };

private:
    bool _send(String msg)
    {
        WiFiClient client;
        client.connect(ip, port);
        client.connect(ip, port);
        bool res = client.connected();
        if (res)
            if (client.print(msg) != msg.length())
                res = 0;
        client.stop();
        return res;
    }
    IPAddress ip;
    uint16_t port;
    static FileTransferManager ftm;
    WiFiClient client;
};

#endif