#include <main.h>
#include <led.h>
#include <motor.h>
#include <debug_print/debug_print.h>

void setup()
{
  sense.begin(onEvent, DEVICE_TYPE, HW_VERSION, FW_VERSION);
  pinMode(RESET_PIN, INPUT_PULLUP);

  DebugLn("Init complete");
}

void updateLeds()
{
}

void loop()
{
  sense.loop();

  static int64_t prevMicros;
  int64_t curMicros = esp_timer_get_time();
  if ((curMicros - prevMicros) > (INPUT_CHECK_MS * 1000))
  {
    uint32_t prevState = sense.getState(0);

    updateLeds();
    prevMicros = curMicros;
  }

  if (digitalRead(RESET_PIN) == 0)
  {
    DebugLn("Resetting the system");
    sense.reset();
  }
}