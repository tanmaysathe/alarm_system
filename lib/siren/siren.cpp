#include <Arduino.h>
#include "siren.h"

void Siren::begin(int motorOnPin, int motorOffPin)
{
    _motorOnPin = motorOnPin;
    _motorOffPin = motorOffPin;

    pinMode(_motorOnPin, OUTPUT);
    pinMode(_motorOffPin, OUTPUT);

    digitalWrite(_motorOnPin, LOW);
    digitalWrite(_motorOffPin, LOW);

    off();
}

uint32_t Siren::getSwitchingDelay()
{
    return _switchingDelayMs;
}

void Siren::setSwitchingDelay(uint32_t ms)
{
    if (ms > 0)
    {
        _switchingDelayMs = ms;
    }
}

uint32_t Siren::getMaxOnTimeMins()
{
    return _maxOnTimeMin;
}

void Siren::setMaxOnTimeMins(uint32_t min)
{
    if (min > 0)
    {
        _maxOnTimeMin = min;
    }
}

void Siren::on()
{
    if (_bStateChanging || _bMotorState)
        return;
    _bMotorState = true;
    digitalWrite(_motorOnPin, HIGH);
    _usLastStatus = esp_timer_get_time();
    _bStateChanging = true;
    _bHasTimedOut = false;
}

void Siren::off()
{
    if (_bStateChanging || !_bMotorState)
        return;
    _bMotorState = false;
    digitalWrite(_motorOffPin, HIGH);
    _usLastStatus = esp_timer_get_time();
    _bStateChanging = true;
    _bHasTimedOut = false;
}

bool Siren::state()
{
    return _bMotorState;
}

bool Siren::stateChanging()
{
    return _bStateChanging;
}

bool Siren::hasTimedOut()
{
    return _bHasTimedOut;
}

int64_t Siren::lastStatusMicros()
{
    return _usLastStatus;
}

void Siren::loop()
{
    int64_t curMicros = esp_timer_get_time();
    if ((curMicros - _usLastStatus) > (_switchingDelayMs * 1000) && _bStateChanging)
    {
        _bStateChanging = false;
        digitalWrite(_motorOnPin, LOW);
        digitalWrite(_motorOffPin, LOW);
    }

    if ((curMicros - _usLastStatus) > (_maxOnTimeMin * 60 * 1000 * 1000))
    {
        off();
        _bHasTimedOut = true;
    }
}
