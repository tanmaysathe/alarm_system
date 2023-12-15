#include <Arduino.h>
#include "floatSensor.h"

void FloatSensor::begin(int pin, bool bActiveLow)
{
    _pin = pin;
    pinMode(_pin, INPUT);
    _bActiveLow = bActiveLow;
    _bState = !_bActiveLow;
    // Initialize array to zero
    for (int i = 0; i < NUM_READINGS; i++)
    {
        readings[i] = bActiveLow;
    }
}

bool FloatSensor::state()
{
    return _bState;
}

void FloatSensor::loop()
{
    int64_t curMicros = esp_timer_get_time();
    int sensorValue = digitalRead(_pin);
    if ((curMicros - prevMicros) > (sampleEveryMilis * 1000))
    {
        readings[index] = sensorValue;
        index = (index + 1) % NUM_READINGS;
        int total = 0;
        for (int i = 0; i < NUM_READINGS; i++)
        {
            total += readings[i];
        }
        if (total > (NUM_READINGS * 0.8))
        {
            _bState = !_bActiveLow;
        }
        if (total < (NUM_READINGS * 0.2))
        {
            _bState = _bActiveLow;
        }
        prevMicros = curMicros;
    }
}
