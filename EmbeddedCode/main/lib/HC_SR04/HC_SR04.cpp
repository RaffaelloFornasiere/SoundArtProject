#include "HC_SR04.h"

//HC_SR04 *HC_SR04::_instance=NULL;
HC_SR04 *HC_SR04::_instance(NULL);

HC_SR04::HC_SR04(int trigger, int echo)
    : _trigger(trigger), _echo(echo), _finished(false)
{
  if (_instance == 0)
    _instance = this;
}

void HC_SR04::begin()
{
  pinMode(_trigger, OUTPUT);
  digitalWrite(_trigger, LOW);
  pinMode(_echo, INPUT);
  attachInterrupt(_echo, _echo_isr, CHANGE);
}

void HC_SR04::start()
{
  _finished = false;

  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);
}

double HC_SR04::getRange()
{
  return (_end - _start) / 58.0;
}

void ICACHE_RAM_ATTR HC_SR04::_echo_isr()
{
  HC_SR04 *_this = HC_SR04::instance();

  switch (digitalRead(_this->_echo))
  {
  case HIGH:
    _this->_start = micros();
    break;
  case LOW:
    _this->_end = micros();
    _this->_finished = true;
    break;
  }
}
