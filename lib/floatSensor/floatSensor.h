#pragma once
#include <Arduino.h>
#include <Preferences.h>

#define NUM_READINGS 10

class FloatSensor
{
protected:
    int64_t prevMicros = 0;
    bool _bState = false, _bActiveLow;
    int _pin;
    const int sampleEveryMilis = 50;
    int readings[NUM_READINGS]; // Array to store readings
    int index = 0;              // Index for the current reading

public:
    void begin(int pin, bool bActiveLow = true);
    bool state();
    void loop();
};
