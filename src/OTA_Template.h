/**
 * OTA_Template.h
 * 
 * Provides all functions for OTA firmware update, WiFi connection management,
 * and web configuration for ESP8266 and ESP32 projects.
 * Include this header to use OTA setup and loop routines in your main sketch.
 */

#ifndef OTA_TEMPLATE_H
#define OTA_TEMPLATE_H

// Ensures the device is connected to WiFi
void ensureWiFiConnection();

// Checks for and performs OTA firmware updates if available
void performOTAUpdate();

// Initializes configuration, WiFi, and web server
void otaSetup();

// Main loop function to handle OTA logic and web server requests
void otaLoop();

#endif // OTA_TEMPLATE_H