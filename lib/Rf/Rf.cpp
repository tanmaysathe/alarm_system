/*
 *  Created on: 13-03-2018
 *  Copyright (c) 2018 Tanmay C. Sathe. All rights reserved.
 *  This file is part of WIFI alarm system v6 2018
 *
 *  Application specific wifi related code
 */
#include <Arduino.h>
#include "Ticker.h"
#include "Rf.h"

int _RF_led;
volatile byte EdgeFound = 0, EdgeType;
volatile unsigned long PulseDuration, LastTime;
Ticker noCode500msTicker;
uint32_t PrevCode = 0;

void IRAM_ATTR edgeDetectedCallback()
{
  EdgeFound = 1;
  EdgeType = digitalRead(12);
  PulseDuration = micros() - LastTime;
  LastTime = micros();
}

void Rf::NoCode500msCallback()
{
  digitalWrite(_RF_led, LOW);
  PrevCode = 0;
}
/**
 * Initialize
 * @param AppConfig *
 * @return bool
 */
void Rf::begin(int RF_in, int RF_led)
{
  _RF_in = RF_in;
  _RF_led = RF_led;
  pinMode(_RF_in, INPUT_PULLUP);
  pinMode(_RF_led, OUTPUT);
  digitalWrite(_RF_led, LOW);
  RFDataIndex = 0;
  attachInterrupt(digitalPinToInterrupt(_RF_in), edgeDetectedCallback, CHANGE);
  // PRINT("RF decoder init complete\n");
}

void Rf::loop()
{
  if (Code)
  {
    PrevCode = Code;
    Code = 0;
  }

  if (EdgeFound)
  {
    EdgeFound = 0;
    if (!EdgeType)
    {
      if (PulseDuration > 150 && PulseDuration < 1800)
      {
        RFData[RFDataIndex++] = PulseDuration;
        if (RFDataIndex > 25)
          RFDataIndex = 0;
      }
      else
      {
        RFDataIndex = 0;
      }
    } // !EdgeType
    else
    {
      if (PulseDuration > 5000)
      {
        if (RFDataIndex == 25)
          CheckRFData();
        RFDataIndex = 0;
      }
    }
  }
}

void Rf::CheckRFData()
{
  byte i;                                                // RFDataIndex is kept in range from 0 to 24 before call to CheckRFData() function
  unsigned long BitLength = RFData[0] < 300 ? 150 : 300; // If first bit is less than 300uS, check following bits as if transmitter has 2.2M resistor for PT2262, else check for 4.7M resistor

  for (i = 0; i < 25; i++)
  {
    if (RFData[i] > BitLength && RFData[i] < (BitLength * 2))
    {
      Code <<= 1;
    }
    else if (RFData[i] > (BitLength * 3) && RFData[i] < (BitLength * 6))
    {
      Code <<= 1;
      Code++;
    }
    else
    {
      break;
    }
  }

  if (i == 25)
  {
    // Turn on rf LED & Turn off rfLED after 500ms of inactivity
    digitalWrite(_RF_led, HIGH);
    noCode500msTicker.detach();
    noCode500msTicker.once_ms(500, Rf::NoCode500msCallback);
    Code >>= 1;         // Remove Sync bit
    Code &= 0x00FFFFFF; // Ignore MSB 8 bits
    if (PrevCode == Code)
      Code = 0;
  }
  else
    Code = 0;
}
