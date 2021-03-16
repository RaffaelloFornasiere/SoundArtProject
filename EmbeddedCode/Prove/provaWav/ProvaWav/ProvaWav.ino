#include "SD.h"
#define SD_ChipSelectPin D8
#include <TMRpcm.h>
#include <SPI.h>

TMRpcm tmrpcm;

void setup() {
  tmrpcm.speakerPin = 9;
  Serial.begin(115200);
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }

  tmrpcm.setVolume(6);
  tmrpcm.play("guitartune.wav");
}

void loop() {  }
