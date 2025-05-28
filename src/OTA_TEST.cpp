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
 * Author: R. Zuehlsdorff, 2025
 */

#include <Arduino.h>
#include "config.h"
#include "OTA_WebConfig.h"
#include "OTA_Template.h"

#define DEBUG true

#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif



/**
 * Dummy setup function for custom initializations.
 * You can add your own initialization or startup conditions here.
 */
void userSetup() {
  // TODO: Insert your own initialization code here
  registerCustomEndpoint("/hello", []() {
    server.send(200, "text/plain", "Hello, world!");
    }, HTTP_GET);
}

/**
 * Dummy loop function for custom cyclic tasks.
 * This function is called in every loop() iteration.
 */
void userLoop() {
  // TODO: Insert your own cyclic tasks here
}

/**
 * Sets up the ESP8266 by initializing serial communication,
 * then calls OTA and user setup functions.
 */
void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(1000);

  Serial.println();
  Serial.println(String("Starting ") + APPNAME + " ...");

  otaSetup();   // OTA-related initialization (now in OTA_Template.cpp)

  userSetup();  // Custom initialization
}

/**
 * Main loop that handles OTA logic and user-defined cyclic tasks.
 */
void loop() {
  otaLoop();    // OTA-related tasks (now in OTA_Template.cpp)
  userLoop();   // Custom cyclic tasks
}
