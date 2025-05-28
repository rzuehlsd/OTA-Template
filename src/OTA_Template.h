/**
 * OTA_Template.h
 * 
 * Provides all functions for OTA firmware update, WiFi connection management,
 * and web configuration for ESP8266 projects. 
 * Include this header to use OTA setup and loop routines in your main sketch.
 */

#ifndef OTA_TEMPLATE_H
#define OTA_TEMPLATE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "config.h"
#include "OTA_WebConfig.h"

// Ensures the device is connected to WiFi
void ensureWiFiConnection();

// Indicates the OTA update status via LED and serial output
void indicateUpdateStatus(t_httpUpdate_return ret);

// Checks for and performs OTA firmware updates if available
void performOTAUpdate();

// Initializes configuration, WiFi, and web server
void otaSetup();

// Main loop function to handle OTA logic and web server requests
void otaLoop();

#endif // OTA_TEMPLATE_H