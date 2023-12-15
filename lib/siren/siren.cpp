#include <Arduino.h>
#include "siren.h"

#define BEEP_ON_TIME_MS 50
#define BEEP_OFF_TIME_MS 300

void Siren::begin(int sirenPin)
{
    _sirenPin = sirenPin;

    pinMode(_sirenPin, OUTPUT);
    digitalWrite(_sirenPin, LOW);
    off();
}

void Siren::on()
{
    curBeepSequence = 0;
    digitalWrite(_sirenPin, HIGH);
    _usLastStatus = esp_timer_get_time();
}

void Siren::off()
{
    curBeepSequence = 0;
    digitalWrite(_sirenPin, LOW);
    _usLastStatus = esp_timer_get_time();
}

void Siren::beep(uint8_t beepCount)
{
    curBeepSequence = ((beepCount - 1) * 2) + 1;
}

bool Siren::state()
{
    return digitalRead(_sirenPin);
}

bool Siren::inBeepSequence()
{
    return curBeepSequence;
}

void Siren::loop()
{
    int64_t curMicros = esp_timer_get_time();

    // Beep beep sounding non-blocking
    if (curBeepSequence)
    {
        if (curBeepSequence % 2)
        {
            if ((curMicros - _usLastStatus) > (BEEP_OFF_TIME_MS * 1000))
            {
                digitalWrite(_sirenPin, HIGH);
                _usLastStatus = esp_timer_get_time();
                curBeepSequence--;
            }
        }
        else
        {
            if ((curMicros - _usLastStatus) > (BEEP_ON_TIME_MS * 1000))
            {
                digitalWrite(_sirenPin, LOW);
                _usLastStatus = esp_timer_get_time();
                curBeepSequence--;
            }
        }
    }

    if ((curMicros - _usLastStatus) > (_maxOnTimeSec * 1000 * 1000))
    {
        off();
    }
}
