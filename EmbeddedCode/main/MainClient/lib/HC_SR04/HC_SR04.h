#ifndef HC_SR04_H
#define HC_SR04_H

#include <Arduino.h>

#define CM true
#define INCH false

class HC_SR04 {
  public:
    HC_SR04(int trigger, int echo);
    
    void begin();
    void start();
    bool isFinished(){ return _finished; }
    double getRange();
    static HC_SR04* instance(){ return _instance; }
    
  private:
    static void _echo_isr();
    
    int _trigger, _echo;
    volatile unsigned long _start, _end;
    volatile bool _finished;
    static HC_SR04* _instance;
};

#endif