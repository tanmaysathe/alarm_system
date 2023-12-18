#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <sense/sense.h>
#include <sense/device_types.h>
#include <debug_print/debug_print.h>

#define RESET_PIN 0

// RF signal input
#define RF_IN 4

// Siren & LEDs output
#define SIREN_PIN 25
#define POWER_WIFI_LED 13 // Blink until connected to wifi, stable on connection
#define RF_LED 12         // Blink on RF code received, off by default
#define ARM_LED 14        // Off by default, on if armed, paniced or intrusion detected,

#define WIFI_CHECK_MS 100

// State flags
#define BASE_FLAG 0x00000001

#define ALARM_STATE_INDEX 0
#define SENSOR_MIN_INDEX 1
#define SENSOR_MAX_INDEX 250

// CMD_GET_STATE at index 0 = alarm system state
union AlarmSystemStateParser
{
    uint32_t rawData;
    struct AlarmSystemValues
    {
        uint32_t state : 3;
        uint32_t lastSensor : 23;
        uint32_t lastSensorType : 3;
        uint32_t reserved : 3;
    } val;
};

#define ALARM_STATE_DISARMED 0
#define ALARM_STATE_ARMED 1
#define ALARM_STATE_HOMEARMED 2
#define ALARM_STATE_INTRUSION 3
#define ALARM_STATE_PANIC 4

// CMD_GET_STATE at index 1 to 255 = sensors
union SensorCodeParser
{
    uint32_t rawData;
    struct SensorCodeValues
    {
        uint32_t bEnabled : 1;
        uint32_t type : 3;
        uint32_t rfCode : 23;
        uint32_t reserved : 5;
    } val;
};

#define SENSOR_TYPE_UNKNWON 0
#define SENSOR_TYPE_DISARM 1
#define SENSOR_TYPE_ARM 2
#define SENSOR_TYPE_HOMEARM 3
#define SENSOR_TYPE_INTRUSION 4
#define SENSOR_TYPE_PANIC 5

// Select device ID from device types class
#define DEVICE_TYPE DeviceTypes::securitySystem
// Maximum version allowed is 25.5
// One 1 digit after decimal point will be considered

// HW_VERSION 1.0 = Wifi alarm system based on wroom32 on a general pcb
#define HW_VERSION 1.0
// FW_VERSION 1.0 = Basic wifi alarm system
#define FW_VERSION 1.0

// alarm_system_core.cpp
void alarmSystemBegin();
void alarmSystemLoop();
// events.cpp
void onEvent(JsonDocument &doc);