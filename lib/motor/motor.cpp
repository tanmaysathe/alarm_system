#include <Arduino.h>
#include "motor.h"

void Motor::begin(int motorOnPin, int motorOffPin)
{
    _motorOnPin = motorOnPin;
    _motorOffPin = motorOffPin;

    pinMode(_motorOnPin, OUTPUT);
    pinMode(_motorOffPin, OUTPUT);

    digitalWrite(_motorOnPin, LOW);
    digitalWrite(_motorOffPin, LOW);

    off();
}

uint32_t Motor::getSwitchingDelay()
{
    return _switchingDelayMs;
}

void Motor::setSwitchingDelay(uint32_t ms)
{
    if (ms > 0)
    {
        _switchingDelayMs = ms;
    }
}

uint32_t Motor::getMaxOnTimeMins()
{
    return _maxOnTimeMin;
}

void Motor::setMaxOnTimeMins(uint32_t min)
{
    if (min > 0)
    {
        _maxOnTimeMin = min;
    }
}

void Motor::on()
{
    if (_bStateChanging || _bMotorState)
        return;
    _bMotorState = true;
    digitalWrite(_motorOnPin, HIGH);
    _usLastStatus = esp_timer_get_time();
    _bStateChanging = true;
    _bHasTimedOut = false;
}

void Motor::off()
{
    if (_bStateChanging || !_bMotorState)
        return;
    _bMotorState = false;
    digitalWrite(_motorOffPin, HIGH);
    _usLastStatus = esp_timer_get_time();
    _bStateChanging = true;
    _bHasTimedOut = false;
}

bool Motor::state()
{
    return _bMotorState;
}

bool Motor::stateChanging()
{
    return _bStateChanging;
}

bool Motor::hasTimedOut()
{
    return _bHasTimedOut;
}

int64_t Motor::lastStatusMicros()
{
    return _usLastStatus;
}

void Motor::loop()
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
