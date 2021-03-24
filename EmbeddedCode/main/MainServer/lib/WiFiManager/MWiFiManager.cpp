#include <MWiFiManager.h>

std::vector<APCred> MWiFiManager::GetAPFromSD()
{
    if (!filename.endsWith(".cred"))
    {
        Serial.println(F("NO CREDENTIALS FILE FOUND"));
        return std::vector<APCred>(0);
    }

    File file = SD.open(filename, "r");
    Serial.println("WiFIManager opening file: " + filename);
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
        Serial.println("ssid: " + aux.ssid + " - psw: " + aux.psw);
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
std::vector<APCred> MWiFiManager::AvailableAPs()
{
    std::vector<String> availableNetsStr = AvailableNetworks();
    std::vector<APCred> availableNets(availableNetsStr.size());
    for (size_t i = 0; i < availableNets.size(); i++)
        availableNets[i].ssid = availableNetsStr[i];
    std::sort(availableNets.begin(), availableNets.end(),
              [](const APCred ap1, const APCred ap2) {
                  return ap1.ssid < ap2.ssid;
              });
    return availableNets;
}

bool MWiFiManager::Connect()
{
    connected = false;
    std::vector<APCred> storedNets = GetAPFromSD();
    std::vector<APCred> availableNets = AvailableAPs();

    Serial.println("available nets: ");
    std::for_each(availableNets.begin(), availableNets.end(), [](APCred &ap) { Serial.print(ap.toString()); });
    Serial.println("");

    std::vector<APCred> intersection;
    std::set_intersection(storedNets.begin(), storedNets.end(),
                          availableNets.begin(), availableNets.end(),
                          std::back_inserter(intersection),
                          [](const APCred &ap1, const APCred &ap2) {
                              return ap1.ssid < ap2.ssid;
                          });

    Serial.println("intersection nets: ");
    std::for_each(intersection.begin(), intersection.end(), [](const APCred &ap) { Serial.println(" -> " + ap.toString()); });
    Serial.println("");

    std::vector<std::pair<APCred, int>> pairs;
    pairs.reserve(intersection.size());
    for (auto &i : intersection)
    {
        if (TryConnect(i.ssid, i.psw))
            pairs.push_back(std::make_pair(i, WiFi.RSSI()));
        // else
        //     DeleteAPFromSD(i);
    }
    auto it = std::max_element(pairs.begin(), pairs.end(),
                               [](const std::pair<APCred, int> &ap1, const std::pair<APCred, int> &ap2) {
                                   return ap1.second < ap2.second;
                               });
    APCred bestAP;
    if (it != pairs.end() && it->first != APCred())
    {
        Serial.println("best ap found");
        bestAP = it->first;
        connected = TryConnect(bestAP);
        if (connected)
        {
            Serial.println('\n');
            Serial.println("Connected to ");
            Serial.println(WiFi.SSID());
            Serial.println("IP address:\t");
            Serial.println(WiFi.localIP());
        }
    }
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

std::vector<String> MWiFiManager::AvailableNetworks()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    size_t n = WiFi.scanNetworks();
    std::vector<String> res;
    res.reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res.push_back(WiFi.SSID(i));
        res.back().trim();
    }
    WiFi.scanDelete();

    return res;
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