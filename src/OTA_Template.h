/**
 * OTA_Template.h
 * 
 * Provides all functions for OTA firmware update, WiFi connection management,
 * and web configuration for ESP8266 and ESP32 projects.
 * Include this header to use OTA setup and loop routines in your main sketch.
 *
 * Author: R. Zuehlsdorff
 * Copyright 2025
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OTA_TEMPLATE_H
#define OTA_TEMPLATE_H

#include "OTA_WebConfig.h" // Include the web configuration header for web server handling

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Default to GPIO2 if not defined, adjust as needed for your board
#endif

// Initializes configuration, WiFi, and web server
// See OTA_WebConfig.h for OTAConfig definition
void otaSetup(const OTAConfig &defaults); // Changed to reference

// Main loop function to handle OTA logic and web server requests
void otaLoop();

#endif // OTA_TEMPLATE_H