#include <main.h>
#include <debug_print/debug_print.h>

void onEvent(JsonDocument &doc)
{
    uint32_t state;
    int cmd = doc["cmd"];

    switch (cmd)
    {
    case CMD_SET_STATE:
        state = doc["state"] | 0;
        DebugF("State updated from %d = %d\n", doc["src"], state);
        break;
    default:
        break;
    }
}
