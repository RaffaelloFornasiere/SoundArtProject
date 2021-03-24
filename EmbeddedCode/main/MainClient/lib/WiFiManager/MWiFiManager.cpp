#include <MWiFiManager.h>

std::vector<APCred> MWiFiManager::GetAPFromSD()
{
    if (!filename.endsWith(".cred"))
    {
        Serial.println(F("NO CREDENTIALS FILE FOUND"));
        return std::vector<APCred>(0);
    }

    File file = SD.open(filename, "r");
    //Serial.println("WiFIManager opening file: " + filename);
    if (!file)
    {
        Serial.println(F("WIFIMANAGER_ERROR_OPENING_FILE"));
        return std::vector<APCred>(0);
    }

    std::vector<APCred> storedNets;
    Serial.println("SD's nets:");
    while (file.available())
    {
        APCred aux;
        aux.FromStream(file);
        //Serial.println("ssid: " + aux.ssid + " - psw: " + aux.psw);
        if (aux != APCred())
            storedNets.emplace_back(aux.ssid, aux.psw);
    }
    file.close();
    std::sort(storedNets.begin(), storedNets.end(),
              [](const APCred &ap1, const APCred &ap2) {
                  return ap1.ssid < ap2.ssid;
              });
    return storedNets;
}
// std::vector<String> MWiFiManager::AvailableNetworks()
// {
//     WiFi.mode(WIFI_STA);
//     // really needed?
//     WiFi.disconnect();
//     size_t n = WiFi.scanNetworks();
//     std::vector<String> res;
//     res.reserve(n);
//     for (size_t i = 0; i < n; i++)
//     {
//         res.push_back(WiFi.SSID(i));
//         res.back().trim();
//     }
//     WiFi.scanDelete();

//     return res;
// }
// std::vector<APCred> MWiFiManager::AvailableAPs()
// {
//     std::vector<String> availableNetsStr = AvailableNetworks();
//     std::vector<APCred> availableNets(availableNetsStr.size());
//     for (size_t i = 0; i < availableNets.size(); i++)
//         availableNets[i].ssid = availableNetsStr[i];
//     std::sort(availableNets.begin(), availableNets.end(),
//               [](const APCred &ap1, const APCred &ap2) {
//                   return ap1.ssid < ap2.ssid;
//               });
//     return availableNets;
// }

std::vector<std::pair<APCred, int>> MWiFiManager::ScanWifis()
{
    size_t n = WiFi.scanNetworks();
    std::vector<std::pair<APCred, int>> res(n);
    for (size_t i = 0; i < n; i++)
        res[i] = std::make_pair(APCred(WiFi.SSID(i), ""), WiFi.RSSI(i));
    std::sort(res.begin(), res.end(), [](const std::pair<APCred, int> &w1, const std::pair<APCred, int> &w2) {
        return w1.second > w1.second;
    });
    return res;
}

bool MWiFiManager::Connect()
{
    connected = false;

    std::vector<APCred> storedNets = GetAPFromSD();
    Serial.println("storedNets: ");
    std::for_each(storedNets.begin(), storedNets.end(), [](const APCred &ap) {
        Serial.println("--> SSID: " + ap.toString());
    });
    Serial.println("\n");

    std::vector<std::pair<APCred, int>> scan = ScanWifis();
    Serial.println("available nets: ");
    std::for_each(scan.begin(), scan.end(), [](const std::pair<APCred, int> &ap) {
        Serial.println("--> SSID: " + ap.first.toString() + " RSSI: " + String(ap.second));
    });
    Serial.println("");

    for (auto i : scan)
    {
        APCred ap = *std::find_if(storedNets.begin(), storedNets.end(), [&](const APCred &ap) {
            return i.first.ssid == ap.ssid;
        });
        if ((connected = TryConnect(ap)))
            break;
    }

    if (connected)
        Serial.println("Connected to: " + WiFi.SSID() + ", IP: " + WiFi.localIP().toString()+ "\n");

    return connected;
}

void MWiFiManager::SetAPMode(IPAddress ip)
{
    WiFi.mode(WIFI_AP_STA); //need both to serve the webpage and take commands via tcp
    IPAddress gateway(1, 2, 3, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP("SoundArtAP"); //Access point is open
    Serial.println("AP_STA Mode setted");
    delay(1000);
}

void MWiFiManager::ClearAll()
{
    File root = SD.open("/");
    File file = file.openNextFile();
    while (file)
    {
        while (file.isDirectory())
        {
            file.close();
            file = root.openNextFile();
        }

        String fName = file.name();
        file.close();
        file = root.openNextFile();
        if (fName.endsWith(".cred"))
            SD.remove(fName);
    }
}
WiFiComp MWiFiManager::SearchAPInSD(const APCred &ap)
{
    File file = SD.open(filename, "r");
    //Serial.println("WiFIManager opening file: " + filename);
    if (!file)
    {
        Serial.println(F("WIFIMANAGER_ERROR_OPENING_FILE"));
        return WiFiComp::Different;
    }
    WiFiComp res = WiFiComp::Different;
    while (file.available() && res != WiFiComp::Different)
    {
        APCred aux;
        aux.FromStream(file);
        if (aux == ap)
            res = WiFiComp::Different;
        else if (aux.ssid == ap.ssid)
            res = WiFiComp::SameSSID;
    }
    file.close();
    return res;
}

void MWiFiManager::SetNewWiFi(const String &ssid, const String &psw)
{
    WiFiComp exist = SearchAPInSD(APCred(ssid, psw));
    if (exist == WiFiComp::Equals)
        return;
    if (exist == WiFiComp::SameSSID)
        DeleteAPFromSD(APCred(ssid, psw));

    File file = SD.open(filename, "a");
    if (!file)
        return;
    file.println("");
    file.println(ssid);
    file.println(psw);
    file.close();
}

void MWiFiManager::DeleteAPFromSD(const APCred &ap)
{
    Serial.println("deleting password: " + ap.ssid);
    std::vector<APCred> nets = GetAPFromSD();
    File file = SD.open(filename, "w");
    for (auto i : nets)
    {
        if (i.ssid != ap.ssid)
        {
            file.print(ap.ssid + '\n');
            file.print(ap.psw + '\n');
        }
    }
}

bool MWiFiManager::TryConnect(const String &ssid, const String &psw)
{
    WiFi.begin(ssid, psw);
    WiFi.waitForConnectResult(10000);
    int res = WiFi.isConnected();
    return res;
}