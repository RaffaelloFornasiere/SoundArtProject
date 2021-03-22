
#include <global.h>

void handleSetVolume()
{
    int res = 1; /*devices[server.arg("device").toInt()]
                  ->SetVolume(server.arg("volume").toInt());*/
    String s = "Set volume: device " + String(server.arg("device").toInt()) + " volume " + String(server.arg("volume").toInt());
    Serial.println(s);
    server.send(res ? 200 : 500);
}
void handleDeleteAudio()
{
    int res = 1; //SD.remove("/" + server.arg("audio"));
    String s = "Delte audio: " + String(server.arg("audio"));
    Serial.println(s);
    server.send(res ? 200 : 500);
}

void handleTestDevice()
{
    int res = 1; /*devices[server.arg("device").toInt()]
                  ->SetAudio(server.arg("audio"));*/
    String s = "Test device " + String(server.arg("device").toInt());
    Serial.println(s);
    server.send(res ? 200 : 500);
}

void handleSetAudio()
{
    int res = 1; /*devices[server.arg("device").toInt()]
                  ->SetLoop(server.arg("audio"));*/
    String s = "Set audio: device " + String(server.arg("device").toInt()) + " volume " + String(server.arg("audio"));
    Serial.println(s);
    server.send(res ? 200 : 500);
}
void handleSetLoop()
{
    int res = 1; /*devices[server.arg("device").toInt()]
                  ->SetLoop(server.arg("loop").toInt());*/
    String s = "Set loop: device " + String(server.arg("device").toInt()) + " volume " + String(server.arg("loop").toInt());
    Serial.println(s);
    server.send(res ? 200 : 500);
}

void handleConnectTo()
{
    String ssid = server.arg("ssid"), psw = server.arg("psw");
    Serial.println("connecting to wifi " + ssid + " " + psw);
    
    int res = wifiManager.TryConnect(ssid, psw);
    if (res)
    {
        webpage = "/index.html";
        wifiManager.SetNewWiFi(ssid, psw);
    }
    //wifiManager.set
    
    server.send(res ? 200 : 1001, "text/plain", WiFi.localIP().toString().c_str());
}
void hanldeClearWifiSettings()
{
    wifiManager.ClearAll();
    server.send(200);
}

void handleSetNewWifi()
{
    wifiManager.SetNewWiFi(server.arg("ssid"), server.arg("psw"));
    server.send(200);
}

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += webpage;                       // If a folder is requested, send the index file
    String contentType = getContentType(path); // Get the MIME type
    String pathWithGz = path + ".gz";
    if (SD.exists(pathWithGz) || SD.exists(path))
    {
        // If the file exists, either as a compressed archive, or normal
        if (SD.exists(pathWithGz))      // If there's a compressed version available
            path = pathWithGz;          // Use the compressed verion
        File file = SD.open(path, "r"); // Open the file
        // size_t sent =
        server.streamFile(file, contentType); // Send it to the client
        file.close();                         // Close the file again
        Serial.println(String("\tSent file: ") + path);
        return true;
    }
    Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
    return false;
}


void handleFileUpload()
{ // upload a new file to the SD
    static File fsUploadFile;
    HTTPUpload &upload = server.upload();
 
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");
        Serial.println(filename);
        fsUploadFile = SD.open(filename, "w"); // Open the file for writing in SD (create if it doesn't exist)
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
            server.send(200);
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

