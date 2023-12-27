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
    static AlarmSystemStateParser prevState;
    AlarmSystemStateParser curState;
    curState.rawData = sense.getState(ALARM_STATE_INDEX);

    // Check for RF command
    rf.loop();
    if (rf.Code)
    {
        uint8_t sensorType = getSensorType(rf.Code);
        switch (sensorType)
        {
        case SENSOR_TYPE_DISARM:
            curState.val.state = ALARM_STATE_DISARMED;
            break;
        case SENSOR_TYPE_ARM:
            curState.val.state = ALARM_STATE_ARMED;
            break;
        case SENSOR_TYPE_HOMEARM:
            curState.val.state = ALARM_STATE_ARMED;
            break;
        case SENSOR_TYPE_INTRUSION:
            curState.val.state = ALARM_STATE_INTRUSION;
            break;
        case SENSOR_TYPE_PANIC:
            curState.val.state = ALARM_STATE_PANIC;
            break;
        }
        curState.val.lastSensor = rf.Code;
        curState.val.lastSensorType = sensorType;
        DebugF("state=%X\n", curState.rawData);
    }

    // If state is changed through RF or mqtt, update siren & arm led state
    if (curState.val.state != prevState.val.state)
    {
        switch (curState.val.state)
        {
        case ALARM_STATE_DISARMED:
            armLed.off();
            siren.beep(2);
            break;
        case ALARM_STATE_ARMED:
            armLed.on();
            siren.beep(1);
            break;
        case ALARM_STATE_HOMEARMED:
            armLed.on();
            siren.beep(1);
            break;
        case ALARM_STATE_INTRUSION:
            siren.on();
            break;
        case ALARM_STATE_PANIC:
            siren.on();
            break;
        }
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
    prevState.rawData = curState.rawData;
}