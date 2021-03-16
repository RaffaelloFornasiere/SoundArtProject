
#include <Arduino.h>
#include <SD.h>
#include <AudioFileSourceSPIFFS.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2SNoDAC.h>
#include <AudioOutputI2S.h>
#include <ESP8266HTTPClient.h>
#include <HC_SR04.h>

HC_SR04 hc(D3, D1);
AudioOutputI2S *outI2s;
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *sdFile;
unsigned long int t;
void setup()
{
  Serial.begin(115200);
  Serial.println("");
  delay(1000);
  //SPIFFS.begin();
  SD.begin(D2);
  /*File f = SD.open("/pno-cs.mp3");
  if(!f)
    Serial.println("cant open file");*/
  //file = new AudioFileSourceSPIFFS("/tone.mp3");
  sdFile = new AudioFileSourceSD("/tone.mp3");

  if (!sdFile->isOpen())
    Serial.println("can't open file");

  //out = new AudioOutputI2SNoDAC();
  outI2s = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();

  //out->SetOversampling(256);
  //out->SetRate(128000);
  //
  if (mp3->begin(sdFile, outI2s))
  
    Serial.println("all ok");
  //mp3->loop();
  outI2s->SetGain(2.0);
  hc.begin();
  t = millis();
  hc.start();
}
bool waiting = 0;
bool active = 0;
void loop()
{
  if (hc.isFinished())
  {
    double dist = hc.getRange();
    if (dist < 20)
    {
      if (!mp3->loop())
      {
        mp3->stop();
        Serial.println("finito");
        sdFile->close();
        sdFile->open("/tone.mp3");
        Serial.println("file aperto nuovamente");

        if (mp3->begin(sdFile, outI2s))

          Serial.println("all ok");
          outI2s->SetGain(2.0);
      }
    }
    hc.start();
  }

}
