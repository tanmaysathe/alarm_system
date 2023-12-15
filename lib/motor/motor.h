#pragma once
#include <Arduino.h>
#include <Preferences.h>

class Motor
{
protected:
    bool _bMotorState, _bStateChanging = false, _bHasTimedOut = false;
    int _motorOnPin, _motorOffPin;
    uint32_t _switchingDelayMs = 1000, _maxOnTimeMin = 30;
    int64_t _usLastStatus = 0;

public:
    void begin(int motorOnPin, int motorOffPin);

    uint32_t getSwitchingDelay();
    void setSwitchingDelay(uint32_t ms);

    uint32_t getMaxOnTimeMins();
    void setMaxOnTimeMins(uint32_t min);

    void on();
    void off();
    bool state();
    bool stateChanging();
    bool hasTimedOut();

    int64_t lastStatusMicros();

    void loop();
};
