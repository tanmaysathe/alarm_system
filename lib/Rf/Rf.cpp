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

int _RF_in, _RF_led;
volatile byte EdgeFound = 0, EdgeType;
volatile unsigned long PulseDuration, LastTime;
Ticker noCode500msTicker;
uint32_t PrevCode = 0;

Rf rf;

void IRAM_ATTR edgeDetectedCallback()
{
  EdgeFound = 1;
  EdgeType = digitalRead(_RF_in);
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
  attachInterrupt(_RF_in, edgeDetectedCallback, CHANGE);
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
  const unsigned long coarseBoundaryBit0Bit1 = 800;
  unsigned long avg0 = 0, avg1 = 0, min0 = 5000, min1 = 5000, max0 = 0, max1 = 0;
  int bitCnt0 = 0, bitCnt1 = 0;
  uint8_t i; // RFDataIndex is kept in range from 0 to 24 before call to CheckRFData() function
  for (i = 0; i < 25; i++)
  {
    if (RFData[i] < coarseBoundaryBit0Bit1)
    {
      avg0 += RFData[i];
      if (RFData[i] < min0)
        min0 = RFData[i];
      if (RFData[i] > max0)
        max0 = RFData[i];
      bitCnt0++;
    }
    else
    {
      avg1 += RFData[i];
      if (RFData[i] < min1)
        min1 = RFData[i];
      if (RFData[i] > max1)
        max1 = RFData[i];
      bitCnt1++;
    }
  }

  if (!bitCnt0 || !bitCnt1 || !min0 || !min1)
  {
    return;
  }

  avg0 /= bitCnt0;
  avg1 /= bitCnt1;

  unsigned long bit0Low = avg0 * 0.6;
  unsigned long bit0High = avg0 * 1.6;
  unsigned long bit1Low = avg1 * 0.6;
  unsigned long bit1High = avg1 * 1.6;

  for (i = 0; i < 25; i++)
  {
    if (RFData[i] > bit0Low && RFData[i] < bit0High)
    {
      Code <<= 1;
    }
    else if (RFData[i] > bit1Low && RFData[i] < bit1High)
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
