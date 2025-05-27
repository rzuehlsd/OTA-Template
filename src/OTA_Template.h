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
#include "WebConfig.h"

void ensureWiFiConnection();
void indicateUpdateStatus(t_httpUpdate_return ret);
void performOTAUpdate();
void otaSetup();
void otaLoop();

#endif // OTA_TEMPLATE_H