/*
 *  Created on: 29-03-2018
 *  Copyright (c) 2018 Tanmay C. Sathe. All rights reserved.
 *  This file is part of WIFI alarm system v6 2018
 *
 *  RF Decoder files. Decodes PT2272 / EV1527 codes
 *
 */
#pragma once

#include <Arduino.h>
#include <Ticker.h>

class Rf
{
private:
  /* Default WIFI AP Details */
  unsigned long Data, Temp;
  int _RF_in;
  byte RFDataIndex = 0;
  unsigned long RFData[26] = {0};
  void CheckRFData();
  static void NoCode500msCallback();

public:
  uint32_t Code = 0;
  void begin(int RF_in, int RF_led);
  void loop();
};
