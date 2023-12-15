#pragma once
#include <Arduino.h>
#include <Preferences.h>

class Led
{
protected:
    int64_t prevMicros = 0;
    bool _bIsBlinking;
    uint32_t _onMs, _offMs;
    int _pin;

public:
    void begin(int pin);
    void on();
    void off();
    void toggle();
    void blink(uint32_t onMs = 500, uint32_t offMs = 500);
    void set(bool bState);
    bool state();
    bool isBlinking();
    void loop();
};
