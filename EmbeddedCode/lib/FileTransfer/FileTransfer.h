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

    size_t Send(Stream &s);
    size_t AsyncSend(Stream &s);
    size_t SendWith(WiFiClient& client, Stream& s);
    size_t AsyncSendWith(WiFiClient& client, Stream& s);
    
    bool OpenServer(uint16_t print);
    bool WaitForNewConnection(clock_t timeout = 5000);
    bool OpenNewConnection(IPAddress ip, uint16_t port, clock_t = 5000);
    bool OpenNewConnection(const String &remoteIp, uint16_t remotePort)
    {
        IPAddress ip;
        ip.fromString(remoteIp);
        return OpenNewConnection(ip, remotePort);
    }
    
    void CloseConnection();
    
    size_t Read(Stream &s);
    size_t ReadFrom(IPAddress remoteIp, uint16_t remotePort, Stream &s);
    size_t ReadFrom(WiFiClient& client, Stream& s);

    void SetBufLen(size_t _bufLen) { buffLen = std::min(_bufLen, maxBuffLen); }

    static const uint16_t FileTransferPort = 9000;

    WiFiClient remoteClient(){return client;}

private:
    uint16_t port;
    size_t buffLen;
    WiFiServer server;
    WiFiClient client;

    static const size_t maxBuffLen = 2048;
};

#endif