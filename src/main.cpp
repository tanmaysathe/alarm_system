#include <main.h>
#include <led.h>
#include <motor.h>
#include <floatSensor.h>
#include <debug_print/debug_print.h>

Motor corpMotor1, corpMotor2, boreMotor;
FloatSensor upperSensorMed, upperSensorFull, lowerSensorMed, lowerSensorFull;
Led ledUpperFull, ledUpperMed, ledUpperEmpty, ledLowerFull, ledLowerMed, ledLowerEmpty, ledCorp1, ledCorp2, ledBore;

void setup()
{
  sense.begin(onEvent, DEVICE_TYPE, HW_VERSION, FW_VERSION);
  pinMode(RESET_PIN, INPUT_PULLUP);

  ledUpperFull.begin(UPPER_TANK_LED_FULL);
  ledUpperMed.begin(UPPER_TANK_LED_MED);
  ledUpperEmpty.begin(UPPER_TANK_LED_LOW);
  ledLowerFull.begin(LOWER_TANK_LED_FULL);
  ledLowerMed.begin(LOWER_TANK_LED_MED);
  ledLowerEmpty.begin(LOWER_TANK_LED_LOW);
  ledCorp1.begin(CORP1_MOTOR_LED);
  ledCorp2.begin(CORP2_MOTOR_LED);
  ledBore.begin(BORE_MOTOR_LED);

  ledUpperEmpty.on();
  ledLowerEmpty.on();

  corpMotor1.begin(CORP1_MOTOR_START, CORP1_MOTOR_STOP);
  corpMotor2.begin(CORP2_MOTOR_START, CORP2_MOTOR_STOP);
  boreMotor.begin(BORE_MOTOR_START, BORE_MOTOR_STOP);

  lowerSensorMed.begin(LOWER_SENSOR_MED);
  lowerSensorFull.begin(LOWER_SENSOR_FULL);
  upperSensorMed.begin(UPPER_SENSOR_MED);
  upperSensorFull.begin(UPPER_SENSOR_FULL);

  DebugLn("Init complete");
}

void updateLeds()
{
  ledBore.set(boreMotor.state());
  ledCorp1.set(corpMotor1.state());
  ledCorp2.set(corpMotor2.state());
  ledLowerMed.set(lowerSensorMed.state());
  ledLowerFull.set(lowerSensorFull.state());
  ledUpperMed.set(upperSensorMed.state());
  ledUpperFull.set(upperSensorFull.state());
}

void updateState(uint32_t prevState)
{
  // Update state
  uint32_t newState = 0;

  newState |= boreMotor.state() ? FLAG_BORE_MOTOR_ON : 0;
  newState |= prevState & FLAG_BORE_MOTOR_MANUAL;
  newState |= boreMotor.stateChanging() ? FLAG_BORE_MOTOR_CHANGING : 0;
  newState |= boreMotor.hasTimedOut() ? FLAG_BORE_MOTOR_TIMEOUT : 0;

  newState |= corpMotor1.state() ? FLAG_CORP_MOTOR1_ON : 0;
  newState |= prevState & FLAG_CORP_MOTOR1_MANUAL;
  newState |= corpMotor1.stateChanging() ? FLAG_CORP_MOTOR1_CHANGING : 0;
  newState |= corpMotor1.hasTimedOut() ? FLAG_CORP_MOTOR1_TIMEOUT : 0;

  newState |= corpMotor2.state() ? FLAG_CORP_MOTOR2_ON : 0;
  newState |= prevState & FLAG_CORP_MOTOR2_MANUAL;
  newState |= corpMotor2.stateChanging() ? FLAG_CORP_MOTOR2_CHANGING : 0;
  newState |= corpMotor2.hasTimedOut() ? FLAG_CORP_MOTOR2_TIMEOUT : 0;

  newState |= lowerSensorMed.state() ? STATE_FLAG_LOWER_TANK_SENSOR_MED : 0;
  newState |= lowerSensorFull.state() ? STATE_FLAG_LOWER_TANK_SENSOR_FULL : 0;
  newState |= upperSensorMed.state() ? STATE_FLAG_UPPER_TANK_SENSOR_MED : 0;
  newState |= upperSensorFull.state() ? STATE_FLAG_UPPER_TANK_SENSOR_FULL : 0;

  if (prevState != newState)
  {
    sense.setState(0, newState);
  }
}

void loop()
{
  sense.loop();
  ledUpperFull.loop();
  ledUpperMed.loop();
  ledUpperEmpty.loop();
  ledLowerFull.loop();
  ledLowerMed.loop();
  ledLowerEmpty.loop();
  ledCorp1.loop();
  ledCorp2.loop();
  ledBore.loop();

  corpMotor1.loop();
  corpMotor2.loop();
  boreMotor.loop();

  lowerSensorMed.loop();
  lowerSensorFull.loop();
  upperSensorMed.loop();
  upperSensorFull.loop();

  static int64_t prevMicros;
  int64_t curMicros = esp_timer_get_time();
  if ((curMicros - prevMicros) > (INPUT_CHECK_MS * 1000))
  {
    uint32_t prevState = sense.getState(0);

    // If lower tank is empty, stop corp motors & start bore motor
    if (!lowerSensorMed.state())
    {
      if (!(prevState & FLAG_BORE_MOTOR_MANUAL))
        boreMotor.on();
      if (!(prevState & FLAG_CORP_MOTOR1_MANUAL))
        corpMotor1.off();
      if (!(prevState & FLAG_CORP_MOTOR2_MANUAL))
        corpMotor2.off();
    }
    else
    {
      boreMotor.off();
    }

    // If upper tank is empty & lower tank is not empty, turn on corpMotor
    if (!upperSensorFull.state() && lowerSensorMed.state())
    {
      if (!(prevState & FLAG_CORP_MOTOR1_MANUAL))
        corpMotor1.on();
      else if (!(prevState & FLAG_CORP_MOTOR2_MANUAL))
        corpMotor2.on();
    }
    else
    {
      if (!(prevState & FLAG_CORP_MOTOR1_MANUAL))
        corpMotor1.off();
      if (!(prevState & FLAG_CORP_MOTOR1_MANUAL))
        corpMotor2.off();
    }

    // Update motor state if on manual mode
    if (prevState & FLAG_BORE_MOTOR_MANUAL)
      (prevState & FLAG_BORE_MOTOR_ON) ? boreMotor.on() : boreMotor.off();
    if (prevState & FLAG_CORP_MOTOR1_MANUAL)
      (prevState & FLAG_CORP_MOTOR1_ON) ? corpMotor1.on() : corpMotor1.off();
    if (prevState & FLAG_CORP_MOTOR2_MANUAL)
      (prevState & FLAG_CORP_MOTOR2_ON) ? corpMotor2.on() : corpMotor2.off();

    updateLeds();
    updateState(prevState);
    prevMicros = curMicros;
  }

  if (digitalRead(RESET_PIN) == 0)
  {
    DebugLn("Resetting the system");
    sense.reset();
  }
}