#include <main.h>
#include <debug_print/debug_print.h>

void alarmSystemStateChanged(JsonDocument &doc)
{
    uint8_t id = doc["id"] | 255;
    uint8_t source = doc["src"] | EVENT_SOURCE_NONE;

    if (id > 0 || source == EVENT_SOURCE_APP)
        return;

    AlarmSystemStateParser p;
    p.rawData = doc["state"] | 0;

    switch (p.val.state)
    {
    case ALARM_STATE_DISARMED:
        break;
    case ALARM_STATE_ARMED:
        break;
    case ALARM_STATE_HOMEARMED:
        break;
    case ALARM_STATE_INTRUSION:
        break;
    case ALARM_STATE_PANIC:
        break;
    }
}

void onEvent(JsonDocument &doc)
{
    uint32_t state;
    int cmd = doc["cmd"];

    switch (cmd)
    {
    case CMD_SET_STATE:
        alarmSystemStateChanged(doc);
        break;
    default:
        break;
    }
}
