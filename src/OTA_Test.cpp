/* OTA_TEST - Automatic Firmware Update for ESP8266 with Web Configuration
 * ----------------------------------------------------------------------------
 * This program enables an ESP8266 to automatically update its firmware
 * as soon as a newer version is available on an OTA server.
 * 
 * Main features:
 * - On startup, the saved configuration is loaded from EEPROM.
 * - The ESP8266 connects to the configured WiFi network.
 * - A web interface (integrated web server) allows configuration
 *   (WiFi, OTA server, update interval, etc.) to be conveniently changed in the browser.
 * - At regular intervals, the device checks if a new firmware version
 *   is available on the OTA server. If a new version is available, the
 *   update is performed automatically.
 * - The update status is indicated via the internal LED.
 * 
 * Web Configuration:
 * ----------------------------------------------------------------------------
 * Configuration is done via a user-friendly web page provided by the ESP8266 itself.
 * After startup, the device can be accessed in the browser via its IP address.
 * The following settings can be made there:
 *   - WiFi SSID and key
 *   - OTA server address and port
 *   - Enable/disable OTA service
 *   - Update interval (in minutes)
 *   - Firmware information is displayed
 *   - Changes can be saved and the device restarted directly
 * 
 * After saving, the settings are stored in EEPROM and automatically loaded on the next start.
 * 
 * User Extensions:
 * ----------------------------------------------------------------------------
 * To extend the program with your own functionality, implement your code in the
 * userSetup() and userLoop() functions below. These functions are called automatically:
 *   - userSetup(): Called once during startup after OTA and web configuration setup.
 *   - userLoop():  Called in every main loop iteration after OTA and web server handling.
 * 
 * Place your own constants, pin definitions, and global variables at the top of this file,
 * above the userSetup() and userLoop() functions, or in a separate header file if preferred.
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

#include <Arduino.h>
#include "config.h"
#include "OTA_Template.h"

#define DEBUG true

#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif


const OTAConfig defaultOTAConfig = {
    APSSID,                // ssid
    APPSK,                 // password
    OTA_SERVER,            // otaServer
    OTA_PORT,              // otaPort
    OTA_ENABLED,           // otaEnabled
    OTA_UPDATE_INTERVAL,   // otaUpdateInterval
    WEB_SERVER_PORT,       // webServerPort
    APPNAME,               // appname
    FIRMWARE_NAME,         // firmware_name
    FIRMWARE_VERSION,      // firmware_vers
    DESCRIPTION            // description
};

/**
 * Dummy setup function for custom initializations.
 * You can add your own initialization or startup conditions here.
 */
void userSetup() {
  // TODO: Insert your own initialization code here
  registerCustomEndpoint("/hello", []() {
    server.send(200, "text/plain", "Hello, world!");
    }, HTTP_GET);

    pinMode(LED_BUILTIN, OUTPUT);
}

/**
 * Dummy loop function for custom cyclic tasks.
 * This function is called in every loop() iteration.
 */
void userLoop() {
  // TODO: Insert your own cyclic tasks here
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}

/**
 * Sets up the ESP8266 by initializing serial communication,
 * then calls OTA and user setup functions.
 */
void setup() {
  Serial.begin(115200);
  while(!Serial)
  {
    delay(100);
  };

  Serial.println();
  Serial.println(String("Starting ") + APPNAME + " ...");

  otaSetup(defaultOTAConfig);   // Pass by reference, no &
  userSetup();  // Custom initialization
}

/**
 * Main loop that handles OTA logic and user-defined cyclic tasks.
 */
void loop() {
  otaLoop();    // OTA-related tasks (now in OTA_Template.cpp)
  userLoop();   // Custom cyclic tasks
}

