#pragma once
#include <Arduino.h>
#include <Preferences.h>

class Led
{
protected:
    int64_t prevMicros = 0;
    bool _bIsBlinking;
    const uint32_t _onMs = 500, _offMs = 500;
    int _pin;

public:
    void begin(int pin);
    void on();
    void off();
    void toggle();
    void blink();
    void set(bool bState);
    bool state();
    bool isBlinking();
    void loop();
};
