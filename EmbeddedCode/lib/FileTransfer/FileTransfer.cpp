#include "FileTransfer.h"

bool FileTransferManager::OpenNewConnection(IPAddress ip, uint16_t port, clock_t timeout)
{
    client.connect(ip, port);
    clock_t t = millis();
    while (((millis() - t) < timeout) && !client.connected())
        ;

    return client.connected();
}

bool FileTransferManager::OpenServer(uint16_t print)
{
    server.close();
    server.begin(port);
    return true;
}

bool FileTransferManager::WaitForNewConnection(clock_t timeout)
{
    size_t t = millis();
    do
    {
        client = server.available();
    } while (!client.connected() && (millis() - t) < timeout);
    return client.connected();
}

void FileTransferManager::CloseConnection()
{
    client.stop();
}

//Sends file to remote device if connected
size_t FileTransferManager::SendWith(WiFiClient &client, Stream &stream)
{
    this->client = client;
    return Send(stream);
}

size_t FileTransferManager::Send(Stream &stream)
{
    size_t sent = 0;
    clock_t packetTimeout = 3000;
    if (client.connected())
    {

        int i = 0;
        clock_t t = millis();

        size_t os = 0;
        buffLen = 500;
        while (stream.available())
        {
            char c[buffLen];
            char *pc = c;
            size_t toSend = std::min(buffLen, size_t(stream.available()));
            //Serial.println("Pending: " + String(stream.available()));
            //Serial.println(" to send: " + String(toSend));

            stream.readBytes(c, toSend);
            size_t s = 0;
            while (s != toSend)
            {
                pc = &c[s];
                s += client.write(pc, toSend - s);
                sent += s;
            }

            if ((millis() - t) > 1000)
            {
                Serial.println(String(sent - os) + " B/s with bufflen: " + String(buffLen));
                os = sent;
                t = millis();
            }
        }
    }
    return sent;
}

// ****************************************************************************

//reads the incoming file and returns the size of the file
size_t FileTransferManager::Read(Stream &stream)
{
    size_t bytes = 0;
    if (client.connected())
    {
        Serial.println("begin receiving");
        while (client.connected())
        {
            while (client.available())
                bytes += stream.write(client.read());
            stream.flush();
        }

        Serial.println("file received");
    }
    return bytes;
}

size_t FileTransferManager::ReadFrom(IPAddress remoteIp, uint16_t remotePort, Stream &stream)
{
    size_t bytes = 0;
    if (client)
    {
        if (client.remoteIP() != remoteIp || client.remotePort() != remotePort)
            return 0;

        //Serial.println("begin receiving");
        while (client.connected())
        {
            while (client.available())
                bytes += stream.write(client.read());

            stream.flush();
        }
        //Serial.println("file received");
    }
    return bytes;
}

size_t FileTransferManager::ReadFrom(WiFiClient &client, Stream &s)
{
    this->client = client;
    return Read(s);
}