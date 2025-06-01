/**
 * OTA_Template.h
 * 
 * Provides all functions for OTA firmware update, WiFi connection management,
 * and web configuration for ESP8266 and ESP32 projects.
 * Include this header to use OTA setup and loop routines in your main sketch.
 *
 * Author: R. Zuehlsdorff
 * Copyright 2025
 */

#ifndef OTA_TEMPLATE_H
#define OTA_TEMPLATE_H

#include "OTA_WebConfig.h" // Include the web configuration header for web server handling

// Initializes configuration, WiFi, and web server
// See OTA_WebConfig.h for OTAConfig definition
void otaSetup(const OTAConfig &defaults); // Changed to reference

// Main loop function to handle OTA logic and web server requests
void otaLoop();

#endif // OTA_TEMPLATE_H