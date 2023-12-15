#pragma once
#include <Arduino.h>
#include <Preferences.h>

class Siren
{
protected:
    int _sirenPin;
    uint32_t _maxOnTimeSec = 40;
    int64_t _usLastStatus = 0;
    uint8_t curBeepSequence = 0;

public:
    void begin(int sirenPin);
    void on();
    void off();
    void beep(uint8_t beepCount = 1);
    bool state();
    bool inBeepSequence();
    void loop();
};
