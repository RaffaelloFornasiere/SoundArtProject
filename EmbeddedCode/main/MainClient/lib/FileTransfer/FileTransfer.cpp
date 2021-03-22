#include "FileTransfer.h"

//Sends file to remote device if connected
size_t FileTransferManager::Send(IPAddress remoteIp,  Stream &stream)
{
    client.connect(remoteIp, port);
    size_t s = 0;
    if (client.connected())
    {
        while (stream.available())
        {
            size_t toSend = std::min(buffLen, size_t(stream.available()));
            Serial.print("Pending: " + String(stream.available()));
            Serial.println(" to send: " + String(toSend));
            char c[toSend + 1];
            char *pc = c;

            Serial.println("readed:" + String(stream.readBytes(c, toSend)));
            s = client.write(c, toSend);

            s = 0;
            while (s != toSend)
            {
                pc = &c[s];
                s += client.write(pc, toSend - s);
                client.flush();
                Serial.println("sent: " + String(s));
            }
        }
    }
    client.stop();
    return s;
}
// ****************************************************************************

//reads the incoming file and returns the size of the file
size_t FileTransferManager::Read(Stream &stream)
{

    client = server.available();
    size_t bytes = 0;
    if (client)
    {
        //Serial.println("begin receiving");
        while (client.connected())
            while (client.available())
                bytes += stream.write(client.read());
            
        
        //Serial.println("file received");
    }
    return bytes;
}

size_t FileTransferManager::ReadFrom(IPAddress remoteIp, uint16_t remotePort, Stream &stream)
{

    client = server.available();
    size_t bytes = 0;
    if (client)
    {
        if (client.remoteIP() != remoteIp || client.remotePort() != remotePort)
            return 0;

        //Serial.println("begin receiving");
        while (client.connected())
            while (client.available())
                bytes += stream.write(client.read());
        //Serial.println("file received");
    }
    return bytes;
}
