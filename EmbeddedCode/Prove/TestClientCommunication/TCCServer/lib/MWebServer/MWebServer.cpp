#include "MWebServer.h"



String WebServer::getContentType(String filename)
{
    if (filename.endsWith(F(".htm")))
        return F("text/html");
    else if (filename.endsWith(F(".html")))
        return F("text/html");
    else if (filename.endsWith(F(".css")))
        return F("text/css");
    else if (filename.endsWith(F(".js")))
        return F("application/javascript");
    else if (filename.endsWith(F(".png")))
        return F("image/png");
    else if (filename.endsWith(F(".gif")))
        return F("image/gif");
    else if (filename.endsWith(F(".jpg")))
        return F("image/jpeg");
    else if (filename.endsWith(F(".ico")))
        return F("image/x-icon");
    else if (filename.endsWith(F(".xml")))
        return F("text/xml");
    else if (filename.endsWith(F(".pdf")))
        return F("application/x-pdf");
    else if (filename.endsWith(F(".zip")))
        return F("application/x-zip");
    else if (filename.endsWith(F(".gz")))
        return F("application/x-gzip");
    return "text/plain";

} 




bool WebServer::handleFileRead(String path)
{ // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);

    if (path.endsWith("/"))
        path += "index.html";                  // If a folder is requested, send the index file
    String contentType = getContentType(path); // Get the MIME type
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) // If the file exists, either as a compressed archive, or normal
    {
        if (SPIFFS.exists(pathWithGz))                      // If there's a compressed version available
            path = pathWithGz;                              // Use the compressed verion
        File file = SPIFFS.open(path, "r");                 // Open the file
        size_t sent = server.streamFile(file, contentType); // Send it to the client
        file.close();                                       // Close the file again
        Serial.println(String("\tSent file: ") + path);
        return true;
    }
    Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
    return false;
}

void WebServer::handleFileUpload()
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
            //server.sendHeader("Location", "/success.html"); // Redirect the client to the success page
            server.send(204); ///
        }
        else
        {
            Serial.println("File upload failed");
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}