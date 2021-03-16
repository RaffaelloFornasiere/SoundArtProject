
#include "FS.h"

void setup() {

  Serial.begin(1000000);
  Serial.println("");
 
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  Dir dir = SPIFFS.openDir ("");
  while (dir.next ()) {
    
    Serial.println (dir.fileName ());
    Serial.println (dir.fileSize ());
  }
  
}
void loop() {}
