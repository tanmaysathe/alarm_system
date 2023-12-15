#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <sense/sense.h>
#include <sense/device_types.h>
#include <debug_print/debug_print.h>

#define RESET_PIN 0

// RF signal input
#define RF_IN 32

// LED outputs
#define ARM_LED 14              // Off by default, on if armed, paniced or intrusion detected,
#define POWER_WIFI_LED 12       // Blink until connected to mqtt, stable on connection
#define RF_LED 13               // Blink on RF activity

// Motor outputs
#define SIREN 16
#define OUTPUT1 17
#define OUTPUT2 18
#define OUTPUT3 19

// State flags
#define BASE_FLAG 0x00000001

#define FLAG_ARM (BASE_FLAG << 1)
#define FLAG_PANIC (BASE_FLAG << 2)
#define FLAG_INTRUSION (BASE_FLAG << 3)

// Select device ID from device types class
#define DEVICE_TYPE DeviceTypes::securitySystem
// Maximum version allowed is 25.5
// One 1 digit after decimal point will be considered

// HW_VERSION 1.0 = Water level controller with 1 bore, 2 corp motors, 1 upper tank & 1 lower tank
//  1 extra corp motor is for backup purpose
#define HW_VERSION 1.0
#define FW_VERSION 1.0

// events.cpp
void onEvent(JsonDocument &doc);