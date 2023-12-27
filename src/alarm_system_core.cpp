#include <main.h>
#include <Rf.h>
#include <led.h>
#include <siren.h>
#include <WiFi.h>

Siren siren;
Led armLed, wifiLed;

void alarmSystemBegin()
{
    siren.begin(SIREN_PIN);
    rf.begin(RF_IN, RF_LED);
    armLed.begin(ARM_LED);
    wifiLed.begin(POWER_WIFI_LED);
    wifiLed.blink();
}

uint8_t getSensorType(uint32_t rfCode)
{
    SensorCodeParser p;
    for (uint8_t i = SENSOR_MIN_INDEX; i < SENSOR_MAX_INDEX; i++)
    {
        p.rawData = sense.getState(i);
        if (p.val.bEnabled && p.val.rfCode == rfCode)
            return p.val.type;
    }
    return SENSOR_TYPE_UNKNWON;
}

void alarmSystemLoop()
{
    AlarmSystemStateParser curState;
    curState.rawData = sense.getState(ALARM_STATE_INDEX);

    // Check for RF command
    rf.loop();
    if (rf.Code)
    {
        uint8_t sensorType = getSensorType(rf.Code);
        curState.val.lastSensor = rf.Code;
        curState.val.lastSensorType = sensorType;
        curState.val.bStateChanged = false;
        switch (sensorType)
        {
        case SENSOR_TYPE_DISARM:
            if (curState.val.state != ALARM_STATE_DISARMED)
            {
                armLed.off();
                siren.beep(2);
                curState.val.state = ALARM_STATE_DISARMED;
                curState.val.bStateChanged = true;
            }
            break;
        case SENSOR_TYPE_ARM:
            if (curState.val.state != ALARM_STATE_ARMED)
            {
                armLed.on();
                siren.beep(1);
                curState.val.state = ALARM_STATE_ARMED;
                curState.val.bStateChanged = true;
            }
            break;
        case SENSOR_TYPE_HOMEARM:
            if (curState.val.state != ALARM_STATE_HOMEARMED)
            {
                armLed.on();
                siren.beep(1);
                curState.val.state = ALARM_STATE_HOMEARMED;
                curState.val.bStateChanged = true;
            }
            break;
        case SENSOR_TYPE_INTRUSION:
            if (curState.val.state != ALARM_STATE_INTRUSION)
            {
                siren.on();
                curState.val.state = ALARM_STATE_INTRUSION;
                curState.val.bStateChanged = true;
            }
            break;
        case SENSOR_TYPE_PANIC:
            if (curState.val.state != ALARM_STATE_PANIC)
            {
                siren.on();
                curState.val.state = ALARM_STATE_PANIC;
                curState.val.bStateChanged = true;
            }
            break;
        }
        DebugF("state=%X\n", curState.rawData);
        if (curState.val.bStateChanged || curState.val.bSendUnknownRF)
            sense.setState(ALARM_STATE_INDEX, curState.rawData);
    }

    // Refresh wifi led state every 100ms (WIFI_CHECK_MS)
    static int64_t prevMicros;
    int64_t curMicros = esp_timer_get_time();
    if ((curMicros - prevMicros) > (WIFI_CHECK_MS * 1000))
    {
        prevMicros = curMicros;
        WiFi.isConnected() ? wifiLed.on() : wifiLed.blink();
    }

    // Siren, leds loop & refresh prevState
    siren.loop();
    armLed.loop();
    wifiLed.loop();
}