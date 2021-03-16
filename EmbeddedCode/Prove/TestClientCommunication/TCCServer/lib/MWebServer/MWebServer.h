#ifndef MWEBSERVER_H
#define MWEBSERVER_H
#include <ESP8266WebServer.h>


class WebServer
{
public:
    WebServer(String index = "index.html"):server(80){}
    void SetIndexPath(String path);

private:
    String getContentType(String filename);
    bool handleFileRead(String path);
    void handleFileUpload();
    ESP8266WebServer server;


};

#endif