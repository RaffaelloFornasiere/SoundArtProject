#include <Arduino.h>
#include <HC_SR04.h>
#include <math.h>

HC_SR04 hc(D3, D1);

void setup()
{
  Serial.begin(500000);
  hc.begin();
  hc.start();
}



String intToFormattedString(int n, int f)
{
  String res;
  for (int i = 0; i < f - log10(float(n)); i++)
    res += " ";
  res += String(n);
  return res;
}


int detectedDist;
int distance = 30;
unsigned long int debounceT = 100;

void loop()
{
  static int size = 40;
  static std::vector<int> values(size, 0);
  static int index = 0;
  static int oldVal = 0;
  static long unsigned t = millis();
  static bool objDetected = false;
  static bool auxDetect = false;

  if (hc.isFinished())
  {
    values[index] = hc.getRange();
    detectedDist = 0;
    for(int i = 0; i < size; i++)
      detectedDist += values[i];
    detectedDist /= size;
    //Serial.println(" ------ Distance computed: " + String(detectedDist) + " index: " + String(index));
    if (++index >= size)
      index = 0;
    oldVal = values[index];
    hc.start();
  }

  if((detectedDist < distance) != objDetected)
    {
        if(!auxDetect)
            t = millis();
        auxDetect = true;
    }
    else 
        auxDetect = false;

    if(auxDetect && ((millis() - t) > debounceT))
    {
        objDetected = !objDetected;
        Serial.println(objDetected?"detected":"non detected");
    }
}