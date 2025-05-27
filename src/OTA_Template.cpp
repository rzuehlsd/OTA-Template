/**
 * OTA_Template.cpp
 * 
 * This file provides all core functions for:
 *  - Automatic OTA firmware update for ESP8266
 *  - WiFi connection management
 *  - Web configuration interface integration
 * 
 * Use otaSetup() in your setup() and otaLoop() in your loop() to enable
 * automatic firmware updates and web-based configuration in your project.
 * 
 * Configuration Handling:
 * ----------------------------------------------------------------------------
 * On startup, configuration data (WiFi, OTA server, update interval, etc.) is loaded
 * from EEPROM using loadConfig(). If no valid data is found, default values in config.h 
 * are used.
 * 
 * When the user changes settings via the web configuration interface, the new values
 * are saved to EEPROM and take effect after a restart. The web interface allows
 * convenient editing and saving of all relevant parameters.
 * 
 * Functions:
 *  - ensureWiFiConnection(): Ensures WiFi is connected.
 *  - indicateUpdateStatus(): Shows OTA update status via LED.
 *  - performOTAUpdate(): Checks for and performs firmware updates.
 *  - otaSetup(): Initializes configuration, WiFi, and web server.
 *  - otaLoop(): Handles OTA logic and web server requests.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <EEPROM.h>
#include "config.h"
#include "WebConfig.h"

extern Config config;
extern WiFiClient client;

/**
 * Ensures the ESP8266 is connected to WiFi.
 * Retries until a connection is established.
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
 * Indicates the status of the OTA update using the built-in LED.
 * - LED on: Update failed
 * - LED off: No updates available
 * - LED blinks: Update successful
 */
void indicateUpdateStatus(t_httpUpdate_return ret) {
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
      Serial.println("OTA Update completed!");
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
 * Performs an OTA update by connecting to the specified server.
 * Checks if the firmware version matches before updating.
 * Retries for up to 30 seconds if the update fails.
 */
void performOTAUpdate() {
  char path[128];
  char buf[128];
  sprintf(path, "http://%s:%d/updates/%s", config.otaServer, config.otaPort, FIRMWARE_NAME);
  sprintf(buf, "http://%s:%d/version/%s.version", config.otaServer, config.otaPort, FIRMWARE_NAME);

  Serial.printf("Starting OTA update from: %s\n", path);
  Serial.printf("Checking firmware version from: %s\n", buf);

  HTTPClient http;
  if (http.begin(client, buf)) {
    int httpCode = http.GET();
    Serial.printf("HTTP response code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String newVersion = http.getString();
      newVersion.trim();
      Serial.printf("Available firmware version: %s\n", newVersion.c_str());
      if (newVersion == CURRENT_VERSION) {
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

  Serial.println("Starting OTA update...");
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, path);
    indicateUpdateStatus(ret);
    if (ret == HTTP_UPDATE_OK) {
      break;
    }
  }
}

/**
 * Initializes OTA functionality: loads config, connects to WiFi, starts web server, prints version.
 */
void otaSetup() {
  loadConfig(); // Load configuration from EEPROM
  Serial.println("READY - Connecting to WiFi ..");

  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.password);

  ensureWiFiConnection();

  Serial.print(F("Firmware version "));
  Serial.println(CURRENT_VERSION);

  startWebServer(); // Start web configuration
}

/**
 * Handles OTA logic: ensures WiFi, web server, and triggers OTA update if enabled.
 */
void otaLoop() {
  ensureWiFiConnection();
  handleWebServer(); // Handle web server requests
  if(config.otaEnabled) {
    // Check for OTA updates every configured interval
    static unsigned long lastUpdateCheck = 0;
    if (millis() - lastUpdateCheck > config.otaUpdateInterval * 60000) { // Convert minutes to milliseconds
      performOTAUpdate();
      lastUpdateCheck = millis();
    }
  }
}