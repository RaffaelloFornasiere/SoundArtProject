#ifndef FILETRANSFER_H
#define FILETRANSFER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <algorithm>

class FileTransferManager
{
public:
    FileTransferManager(uint16_t port, size_t buffLen = 2000)
        : port(port), buffLen(std::min(buffLen, maxBuffLen)), server(port)
    {
        server.begin();
    };

    size_t Send(IPAddress remoteIp, uint16_t remotePort, Stream &s);
    size_t Send(const String &remoteIp, uint16_t remotePort, Stream &s)
    {
        IPAddress ip;
        ip.fromString(remoteIp);
        return Send(ip, remotePort, s);
    }


    size_t Read(Stream &s);
    size_t ReadFrom(IPAddress remoteIp, uint16_t remotePort, Stream &s);

    void SetBufLen(size_t _bufLen) { buffLen = std::min(_bufLen, maxBuffLen); }

private:
    uint16_t port;
    size_t buffLen;
    WiFiServer server;
    WiFiClient client;

    static const size_t maxBuffLen = 2048;
};

#endif