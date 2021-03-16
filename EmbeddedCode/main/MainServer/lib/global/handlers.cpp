
#include <global.h>

void handleSetVolume()
{
    devices[server.arg("device").toInt()]
        ->SetVolume(server.arg("volume").toInt());
}
void handleDeleteAudio()
{
    SD.remove(server.arg("audio"));
}

void handleTestDevice()
{
    devices[server.arg("device").toInt()]
        ->SetAudio(server.arg("audio"));
}

void handleSetAudio()
{
    devices[server.arg("device").toInt()]
        ->SetLoop(server.arg("audio"));
}
void handleSetLoop()
{
    devices[server.arg("device").toInt()]
        ->SetLoop(server.arg("loop").toInt());
}

void handleConnectTo()
{
    wifiManager.SetNewWiFi(server.arg("ssid"), server.arg("psw"));
    if (wifiManager.Connect())
    {
        server.send(200);
        webpage = "index.html";
    }
    else
    {
        server.send(1001);
    }

}
void hanldeClearWifiSettings()
{
    wifiManager.ClearAll();
}

String getContentType(String filename)
{ // convert the file extension to the MIME type
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += webpage;                       // If a folder is requested, send the index file
    String contentType = getContentType(path); // Get the MIME type
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
    {                                                       // If the file exists, either as a compressed archive, or normal
        if (SPIFFS.exists(pathWithGz))                      // If there's a compressed version available
            path = pathWithGz;                              // Use the compressed verion
        File file = SPIFFS.open(path, "r");                 // Open the file
        // size_t sent = 
        server.streamFile(file, contentType); // Send it to the client
        file.close();                                       // Close the file again
        Serial.println(String("\tSent file: ") + path);
        return true;
    }
    Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
    return false;
}

void handleFileUpload()
{ // upload a new file to the SPIFFS

    HTTPUpload &upload = server.upload();
    File fsUploadFile;
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");
        Serial.println(filename);
        fsUploadFile = SPIFFS.open(filename, "w"); // Open the file for writing in SPIFFS (create if it doesn't exist)
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (fsUploadFile)
        {                         // If the file was successfully created
            fsUploadFile.close(); // Close the file again
            Serial.print("handleFileUpload Size: ");
            Serial.println(upload.totalSize);
            server.sendHeader("Location", "/success.html"); // Redirect the client to the success page
            server.send(303);
        }
        else
        {
            Serial.println("File upload failed");
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}


void SendBroadcastMessage(String message, int port)
{
    WiFiUDP udp; //udp.begin(port);
    udp.beginPacket("192.168.0.255", port);
    Serial.println("Sending: " + message);
    size_t payload = udp.write(message.c_str());
    udp.endPacket();
    Serial.println("bytes sent: " + String(payload));
}