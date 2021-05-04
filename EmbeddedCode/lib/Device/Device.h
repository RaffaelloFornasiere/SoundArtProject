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
#include <LowPassFilter.h>

// USES SD and suppose that is already initialized

class Device
{
public:
    Device() {}

    ~Device() {}
    virtual bool SetVolume(int volume) = 0;
    virtual bool SetLoop(bool on) = 0;
    virtual bool SetAudio(String audio) = 0;
    virtual bool SetDistance(unsigned dist) = 0;
    virtual bool Test() = 0;
    virtual bool clearAudios() = 0;
    virtual bool DeleteAudio(String audioName) = 0;
};

class PhysicalDevice : public Device
{
public:
    PhysicalDevice(uint8_t trigPin = D3, uint8_t echoPin = D1)
        : hc(trigPin, echoPin), distance(100), loop(false), fileName("/Test/test.mp3")
    {
        hc.begin();
        hc.start();
        outI2s.SetGain(0.01);
        detectedDist = 0;
    }

    bool SetVolume(int volume) override;
    bool SetLoop(bool on) override;
    bool SetAudio(String audio) override;
    bool SetDistance(unsigned dist) override;
    bool Test() override;
    bool clearAudios() override;
    bool DeleteAudio(String audioName) override;
    void Run();

private:
    HC_SR04 hc;
    unsigned long distance;
    bool loop;
    String fileName;
    AudioOutputI2S outI2s;
    AudioGeneratorMP3 mp3;
    AudioFileSourceSD sdFile;
    double detectedDist;
    const unsigned debounceT = 150;
};

class RemoteDevice : public Device
{
public:
    RemoteDevice(IPAddress ip, uint16_t port) : ip(ip), port(port) {}
    bool SetVolume(int volume) override;
    bool SetLoop(bool on) override;
    bool SetAudio(String audio) override;
    bool SetDistance(unsigned dist) override;
    bool Test() override;
    bool clearAudios() override;
    bool DeleteAudio(String audioName) override;

private:
    bool _send(String msg)
    {

        client.connect(ip, port);
        bool res = client.connected();
        if (res)
            if (client.print(msg) != msg.length())
                res = 0;
        Serial.println("_send() conn: " + String(res));
        Serial.println("_send() err: " + String(client.getWriteError()));
        client.flush();
        client.stop();
        return res;
    }
    IPAddress ip;
    uint16_t port;
    static FileTransferManager ftm;
    WiFiClient client;
};

#endif