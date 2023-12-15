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
#define ARM_LED 14        // Off by default, on if armed, paniced or intrusion detected,
#define POWER_WIFI_LED 12 // Blink until connected to mqtt, stable on connection
#define RF_LED 13         // Blink on RF activity

// Motor outputs
#define SIREN 16
#define OUTPUT1 17
#define OUTPUT2 18
#define OUTPUT3 19

#define INPUT_CHECK_MS 100

// State flags
#define BASE_FLAG 0x00000001

union SecuritySystemStateParser
{
    uint32_t rawData;
    struct SecuritySystemValues
    {
        uint32_t armState : 2;
        uint32_t bPanic : 1;
        uint32_t bIntrusion : 1;
        uint32_t sensorCount : 8;
        uint32_t reserved : 20;
    } val;
};

#define ARM_STATE_DISARMED 0
#define ARM_STATE_ARMED 1
#define ARM_STATE_HOMEARMED 2
// #define ARM_STATE_RESERVED 3


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