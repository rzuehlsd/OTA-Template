/**
 * OTA_Template.cpp
 *
 * Main implementation file for OTA firmware update, WiFi connection management,
 * and web-based configuration for ESP8266 and ESP32 projects.
 *
 * Features:
 *  - Automatic OTA firmware updates for ESP8266/ESP32
 *  - WiFi connection management and monitoring
 *  - Web-based configuration interface for all relevant parameters
 *
 * Usage:
 * ----------------------------------------------------------------------------
 * - Call otaSetup(defaultConfig) in setup() to initialize configuration, WiFi, 
 *   and web server.
 * - Call otaLoop() in loop() to handle OTA logic and web server requests.
 *
 * Configuration Handling:
 * ----------------------------------------------------------------------------
 * On startup, configuration data (WiFi, OTA server, update interval, etc.)
 * is loaded from EEPROM (loadConfig()). If no valid data is found,
 * default values from a provided OTAConfig structure are used.
 *
 * Changes via the web interface are saved to EEPROM and take effect after restart.
 * The web interface allows convenient editing and saving of all relevant parameters.
 *
 * Included functions:
 *  - ensureWiFiConnection(): Ensures WiFi is connected.
 *  - splitVersion()/compareVersion(): Version string utilities for OTA.
 *  - indicateUpdateStatus(): Shows OTA update status via LED and serial.
 *  - performOTAUpdate(): Checks for and performs firmware updates.
 *  - otaSetup(): Initializes configuration, WiFi, and web server.
 *  - otaLoop(): Handles OTA logic and web server requests.
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
#include <vector>
#include <sstream>
#include "OTA_Template.h"

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266httpUpdate.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <Update.h>
  #include <HTTPUpdate.h>
#endif

extern OTAConfig config;
WiFiClient client;

/**
 * Ensures that the device is connected to WiFi.
 * If not, tries to connect and prints the IP address once connected.
 */
void ensureWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

/**
 * Splits a version string (e.g. "1.2.3") into integer components.
 * Returns a vector with the individual numbers.
 */
std::vector<int> splitVersion(const String& version) {
  std::vector<int> parts;
  int start = 0, end = 0;
  while ((end = version.indexOf('.', start)) != -1) {
    parts.push_back(version.substring(start, end).toInt());
    start = end + 1;
  }
  parts.push_back(version.substring(start).toInt());
  return parts;
}

/**
 * Compares two version strings.
 * @return -1 if v1 < v2, 1 if v1 > v2, 0 if equal.
 */
int compareVersion(const String& v1, const String& v2) {
  std::vector<int> ver1 = splitVersion(v1);
  std::vector<int> ver2 = splitVersion(v2);
  size_t len = std::max(ver1.size(), ver2.size());
  for (size_t i = 0; i < len; ++i) {
    int num1 = (i < ver1.size()) ? ver1[i] : 0;
    int num2 = (i < ver2.size()) ? ver2[i] : 0;
    if (num1 < num2) return -1;
    if (num1 > num2) return 1;
  }
  return 0;
}

/**
 * Shows the status of the OTA update via the LED and serial interface.
 * - On error: LED stays on
 * - No update: LED off
 * - Successful update: LED blinks 5 times
 */
void indicateUpdateStatus(t_httpUpdate_return ret, String vers) {
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      digitalWrite(LED_BUILTIN, HIGH); // Error: LED stays on
      Serial.println("OTA Update failed!");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      digitalWrite(LED_BUILTIN, LOW); // No updates: LED off
      Serial.println("No OTA Update available!");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("OTA Update to version " + vers +  " completed!");
      for (int i = 0; i < 5; i++) { // Success: LED blinks 5 times
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
      }
      break;
  }
}

/**
 * Checks if a new firmware version is available on the OTA server,
 * and performs the update if necessary. Saves the new version to EEPROM.
 */
void performOTAUpdate() {
  String newVersion;
  int comp = -1;
  char path[128];
  char buf[128];
  sprintf(path, "http://%s:%d/updates/%s", config.otaServer, config.otaPort, config.firmware_name);
  sprintf(buf, "http://%s:%d/version/%s.version", config.otaServer, config.otaPort, config.firmware_vers);

  Serial.printf("Starting OTA update from: %s\n", path);
  Serial.printf("Checking firmware version from: %s\n", buf);

  HTTPClient http;
  if (http.begin(client, buf)) {
    int httpCode = http.GET();
    Serial.printf("HTTP response code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      newVersion = http.getString();
      newVersion.trim();
      comp = compareVersion(newVersion, config.firmware_vers);
      Serial.printf("Available firmware version: %s\n", newVersion.c_str());
      if (comp == 0){
        Serial.println("Firmware is already up-to-date.");
        http.end();
        return;
      }
    } else {
      Serial.printf("Failed to check firmware version, HTTP code: %d\n", httpCode);
    }
    http.end();
  } else {
    Serial.println("Failed to connect to version check URL.");
  }

  if(comp > 0) {  // There is a new version on OTA server available
    Serial.printf("New firmware version %s available, current version is %s\n", newVersion.c_str(), config.firmware_vers);
    strcpy(config.firmware_vers, newVersion.c_str());
    saveConfigToEEPROM(); // Save new version to EEPROM
    Serial.println("EEPROM Version updated -> Starting OTA update...");
    unsigned long startTime = millis();
    while (millis() - startTime < config.otaUpdateInterval * 60000) { // Check for updates within the interval
      Serial.printf("Performing OTA update to version %s...\n", newVersion.c_str());
      // Perform the OTA update
      strcpy(config.firmware_vers, newVersion.c_str());
      saveConfigToEEPROM(); // Save new version to EEPROM
      Serial.println("Saving new version to EEPROM...");
      Serial.printf("Updating firmware from %s\n", path);
      #if defined(ESP8266)
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, path);
      #elif defined(ESP32)
        HTTPUpdate httpUpdate;
        t_httpUpdate_return ret = httpUpdate.update(client, path);
      #endif
      indicateUpdateStatus(ret, newVersion);
      if (ret == HTTP_UPDATE_OK) {
        break;
      }
    }
  }
}

/**
 * Initializes the configuration, connects to WiFi, and starts the web server.
 * Loads configuration from EEPROM or uses the provided defaults if not present.
 * Starts the web-based configuration interface.
 */
void otaSetup(const OTAConfig &defaults) {
    loadConfig(&defaults); // Pass address to match loadConfig signature

    Serial.println("READY - Connecting to WiFi ..");
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid, config.password);

    ensureWiFiConnection();

    Serial.print(F("Firmware version "));
    Serial.println(config.firmware_vers);

    startWebServer(); // Start web configuration
}

/**
 * Main loop function to handle OTA logic and web server requests.
 * Ensures WiFi connection, handles web server, and checks for OTA updates.
 */
void otaLoop() {
  ensureWiFiConnection();
  handleWebServer(); // Handle web server requests
  if(config.otaEnabled) {
    // Check for OTA updates every configured interval
    static unsigned long lastUpdateCheck = 0;
    // initial update after start then every otaUpdateInterval minutes
    if ((lastUpdateCheck == 0) || (millis() - lastUpdateCheck > config.otaUpdateInterval * 60000)) { // Convert minutes to milliseconds
      performOTAUpdate();
      lastUpdateCheck = millis();
    }
  }
}