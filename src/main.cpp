#include <main.h>
#include <led.h>
#include <siren.h>
#include <debug_print/debug_print.h>

void setup()
{
  sense.begin(onEvent, DEVICE_TYPE, HW_VERSION, FW_VERSION);
  pinMode(RESET_PIN, INPUT_PULLUP);
  alarmSystemBegin();
  DebugLn("Init complete");
}

void loop()
{
  sense.loop();
  alarmSystemLoop();

  if (digitalRead(RESET_PIN) == 0)
  {
    DebugLn("Resetting the system");
    sense.reset();
  }
}