#include <global.h>

void handleSetVolume()
{
    String s = "Set volume: device " + String(server.arg("device").toInt()) + " volume " + String(server.arg("volume").toInt());
    Serial.println(s);
    int res = devices[server.arg("device").toInt()]
                  ->SetVolume(server.arg("volume").toInt());
    server.send(res ? 200 : 500);
}
void handleDeleteAudio()
{
    String s = "Delte audio: " + String(server.arg("audio"));
    Serial.println(s);
    int res = SD.remove("/" + server.arg("audio"));
    server.send(res ? 200 : 500);
}

void handleTestDevice()
{
    String s = "Test device " + String(server.arg("device").toInt());
    Serial.println(s);
    int res = devices[server.arg("device").toInt()]
                  ->Test();
    server.send(res ? 200 : 500);
}

void handleSetAudio()
{
    String s = "Set audio: device " + String(server.arg("device").toInt()) + " volume " + String(server.arg("audio"));
    Serial.println(s);
    int res = devices[server.arg("device").toInt()]
                  ->SetAudio(server.arg("audio"));
    server.send(res ? 200 : 500);
}
void handleSetLoop()
{
    String s = "Set loop: device " + String(server.arg("device").toInt()) + " loop " + String(server.arg("loop").toInt());
    Serial.println(s);
    int res = devices[server.arg("device").toInt()]
                  ->SetLoop(server.arg("loop").toInt());
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

    server.send(res ? 200 : 1001, "text/plain", WiFi.localIP().toString().c_str());
}
void hanldeClearWifiSettings()
{
    Serial.println("clearing all wifis");
    wifiManager.ClearAll();
    server.send(200);
}

void handleSetNewWifi()
{
    Serial.println("setting new wifi: ");
    wifiManager.SetNewWiFi(server.arg("ssid"), server.arg("psw"));
    server.send(200);
}

String getContentType(String filename)
{
    if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
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

void setupHandlers()
{
    // handlers linking
    server.on("/deleteAudio", HTTP_POST, handleDeleteAudio);
    server.on("/testDevice", HTTP_POST, handleTestDevice);
    server.on("/setVolume", HTTP_POST, handleSetVolume);
    server.on("/setAudio", HTTP_POST, handleSetAudio);
    server.on("/setLoop", HTTP_POST, handleSetLoop);
    server.on("/connectTo", HTTP_POST, handleConnectTo);
    server.on("/ipreq", HTTP_GET, [] {
        String msg = "connected to: " + WiFi.SSID() + " on ip: " +
                     WiFi.localIP().toString();
        server.send(200, "text/plain", msg);
    });
    server.on("/setNewWifi", HTTP_POST, handleSetNewWifi);
    server.on("/clearWifiSettings", HTTP_POST, hanldeClearWifiSettings);
    server.on(
        "/upload/uri", HTTP_POST, // if the client posts to the upload page
        []() { server.send(200); },
        handleFileUpload // Receive and save the file
    );
    server.onNotFound([&]() {
        // If the client requests any file
        // send it if it exists
        // otherwise, respond with a 404 (Not Found) error
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "404: Not Found");
    });
}

void SendBroadcastMessage(String message, uint16_t port)
{
    WiFiUDP udp; //udp.begin(port);
    Serial.println("port: " + String(port));
    udp.beginPacket("255.255.255.255", port);
    Serial.println("Sending: " + message);
    size_t payload = udp.write(message.c_str());
    udp.endPacket();
    Serial.println("bytes sent: " + String(payload));
}

bool EditFileLine(String filename, String content, String newContent)
{
    File file = SD.open(filename, "r");
    File tmp = SD.open(filename + ".tmp", "w");
    if (!file)
    {
        Serial.println("can't open file for editing");
        return 0;
    }
    String s = "";
    while (s.indexOf(content) == -1)
    {
        tmp.print(s);
        tmp.flush(); //
        if (!file.available())
        {
            Serial.println("file finished");
            return 0;
        }
        s = "";
        char c = file.peek();
        while (c != '\n' && file.available())
        {
            c = file.read();
            if (c != '\r')
                s += c;
            c = file.peek();
        }
        file.read();
        s += "\n";
    }
    tmp.println(newContent);

    s = file.readString();
    tmp.print(s);
    //Serial.println("remaining: " + s);
    tmp.close();
    file.close();

    file = SD.open(filename, "w");
    tmp = SD.open(filename + ".tmp", "r");
    while (tmp.available())
    {
        s = "";
        char c = tmp.peek();
        while (c != '\n' && tmp.available())
        {
            c = tmp.read();
            if (c != '\r')
                s += c;
            c = tmp.peek();
        }
        tmp.read();
        s += "\n";

        file.print(s);

        file.flush();
    }
    tmp.close();
    file.close();

    if (!SD.remove("/" + filename + ".tmp"))
        Serial.println("cant find file to be deleted");
    return 1;
}

bool PrepareIndexPage(int nOfDevices)
{
    bool res = 1;
    //Serial.println("nOfDevices: " + String(clients.size() + 1));
    if (EditFileLine("index.html", "var nOfDevices", "var nOfDevices = " + String(nOfDevices) + ";"))
        Serial.println("index.html: nOfDevices updated");
    else
        res = 0;

    //Serial.println("parsing sd card");
    File root = SD.open("/");
    std::vector<String> audios;
    //Serial.println("root: " + String(root.name()));
    File file = root.openNextFile();
    while (file)
    {
        while (file.isDirectory())
        {
            file.close();
            file = root.openNextFile();
        }
        //Serial.println(file.name());
        if (String(file.name()).endsWith(".mp3"))
            audios.push_back(file.name());
        file = root.openNextFile();
    }

    //Serial.println("edit index.html adding " + String(audios.size()) + " mp3s");
    String newLine = "var audios = [";
    if (audios.size())
    {
        for (size_t i = 0; i < audios.size() - 1; i++)
            newLine += '\"' + audios[i] + "\",";
        newLine += '\"' + audios.back() + "\"];";
        if (EditFileLine("index.html", "var audios", newLine))
            Serial.println("index.html: var audios updated");
        else
            res = 0;
    }

    return res;
}

bool PrepareCredentialsPage()
{
    bool res;
    std::vector<std::pair<APCred, int>> nets = wifiManager.ScanWifis();

    String newLine = "var wifis = [";
    for (size_t i = 0; i < nets.size() - 1; i++)
        newLine += '\"' + nets[i].first.ssid + '\"' + String(", ");
    newLine += '\"' + nets.back().first.ssid + "\"];";
    res = EditFileLine("credentials.html", "var wifis", newLine);

    wifiManager.SetAPMode(IPAddress(1, 2, 3, 4));
    return res;
}

bool PrepareWebPage()
{
    bool res = 0;
    if (webpage == "index.html")
    {
        SetupDevices();
        res = PrepareIndexPage(devices.size());
    }
    else if (webpage == "credentials.html")
    {
        wifiManager.SetAPMode();
        res = PrepareCredentialsPage();
    }
    return res;
}

bool SetupDevices()
{
    uint16_t udpPort = 8000;
    std::vector<IPAddress> clients = FindDevices(udpPort);
    devices.resize(clients.size() + 1, nullptr);
    devices[0] = &device;
    for (size_t i = 1; i < clients.size()+1; i++)
        devices[i] = new RemoteDevice(clients[i-1], udpPort);

    return true;
}

std::vector<IPAddress> FindDevices(uint16_t udpPort)
{
    std::vector<IPAddress> clients;
    WiFiUDP udpServer;
    udpServer.begin(udpPort);
    String message = "SoundArtCheckUp:" + String(udpPort);
    SendBroadcastMessage(message, udpPort);
    unsigned long time = millis();

    Serial.println("start waiting for devices [UDP]");
    while ((millis() - time < 10000))
    {
        if (udpServer.parsePacket())
        {

            Serial.println("i've got packet");
            if (udpServer.available())
            {
                if (udpServer.readStringUntil('\n') == "soundArtClientReply")
                    if (std::find(clients.begin(), clients.end(), udpServer.remoteIP()) == clients.end())
                        clients.push_back(udpServer.remoteIP());

                //discard other characters
                while (udpServer.available())
                    udpServer.read();
            }
        }
    }
    Serial.println("stop waiting for devices [UDP]");
    Serial.println("found " + String(clients.size()) + " devices");
    return clients;
}