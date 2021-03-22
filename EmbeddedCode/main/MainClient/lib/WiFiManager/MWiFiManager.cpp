#include <MWiFiManager.h>

bool MWiFiManager::Connect()
{
    WiFi.mode(WIFI_STA);
    File file = SD.open(filename, "r");
    if (!file)
    {
        DEBUG_PRINTLN(F("WIFIMANAGER_ERROR_OPENING_FILE"));
        return 0;
    }

    if (file.available())
    {
        String fileID = file.readStringUntil('\n');
        if (fileID != _fileID)
        {
            DEBUG_PRINTLN(F("WIFIMANAGER_ERROR_FILE_ID"));
            return 0;
        }
    }

    std::vector<APCred> aps;
    while (file.available())
    {
        APCred aux;
        aux.FromStream(file);
        wifiMulti.addAP(aux.ssid.c_str(), aux.psw.c_str());
    }

    DEBUG_PRINTLN("Connecting ...");
    uint8_t secs = (millis() / 1000);
    connected = 0;
    while (!(connected = (wifiMulti.run() == WL_CONNECTED)))
    { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(1000);
        DEBUG_PRINTLN('.');
        if (millis() - secs * 1000 > 10000)
            break;
    }

    DEBUG_PRINTLN('\n');
    DEBUG_PRINTLN("Connected to ");
    DEBUG_PRINTLN(WiFi.SSID()); // Tell us what network we're connected to
    DEBUG_PRINTLN("IP address:\t");
    DEBUG_PRINTLN(WiFi.localIP());

    return connected;
}

void MWiFiManager::SetAPMode(IPAddress ip)
{
    WiFi.mode(WIFI_AP_STA); //need both to serve the webpage and take commands via tcp
    IPAddress gateway(1, 2, 3, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP("SoundArtAP", "SApsw2021."); //Access point is open
    delay(1000);
}

std::vector<String> MWiFiManager::AvailableNetworks()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    size_t n = WiFi.scanNetworks();
    std::vector<String> res; res.reserve(n);
    for(size_t i = 0; i < n; i++)
        res.push_back(WiFi.SSID(i));
    
    return res;    
}
void MWiFiManager::ClearAll()
{
    File file = SD.open("/");
        file = file.openNextFile();
        while (file)
        {
            while (file.isDirectory())
                file = file.openNextFile();
            String fName = file.name();
            if (fName.endsWith(".cred"))
            {
                file = file.openNextFile();
                SD.remove(fName);
            }
            else
            {
                file = file.openNextFile();
            }
        }
}

void MWiFiManager::SetNewWiFi(const String& ssid, const String& psw)
{
    File file = SD.open("filename", "a");
    file.println("");
    file.println(ssid);
    file.println(psw);
    file.close();
}