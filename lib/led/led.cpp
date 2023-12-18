#include <Arduino.h>
#include "led.h"

void Led::begin(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    off();
    _bIsBlinking = false;
}

void Led::on()
{
    _bIsBlinking = false;
    digitalWrite(_pin, HIGH);
}

void Led::off()
{
    _bIsBlinking = false;
    digitalWrite(_pin, LOW);
}

void Led::toggle()
{
    _bIsBlinking = false;
    digitalWrite(_pin, !digitalRead(_pin));
}

void Led::blink()
{
    _bIsBlinking = true;
}

bool Led::isBlinking()
{
    return _bIsBlinking;
}

void Led::set(bool bState)
{
    digitalWrite(_pin, bState);
}

bool Led::state()
{
    return digitalRead(_pin);
}

void Led::loop()
{
    if (_bIsBlinking)
    {
        int64_t curMicros = esp_timer_get_time();
        bool bLedIsOn = state();
        if (bLedIsOn)
        {
            if ((curMicros - prevMicros) > (_offMs * 1000))
            {
                digitalWrite(_pin, !digitalRead(_pin));
            }
        }
        else
        {
            if ((curMicros - prevMicros) > (_onMs * 1000))
            {
                digitalWrite(_pin, !digitalRead(_pin));
            }
        }
        prevMicros = curMicros;
    }
}
