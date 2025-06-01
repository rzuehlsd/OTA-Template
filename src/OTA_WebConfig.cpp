/**
 * OTA_WebConfig.cpp
 *
 * This file implements all functions for managing device configuration,
 * providing web server endpoints, and saving/loading settings in EEPROM
 * for ESP8266/ESP32 projects using the OTA Template.
 *
 * Features:
 *  - Defines and manages the OTAConfig structure, which holds all runtime configuration.
 *  - Loads configuration from EEPROM on startup, or uses a provided default OTAConfig struct if no valid data is found.
 *  - Saves configuration changes to EEPROM for persistence across reboots.
 *  - Provides a web-based configuration interface, including HTML form generation and HTTP endpoint handlers.
 *  - Allows registration of custom web endpoints for user extensions.
 *  - Integrates with the main OTA_Template logic for seamless configuration and update management.
 *
 * Usage:
 *  - Call loadConfig() with a default OTAConfig struct to initialize configuration at startup.
 *  - Use startWebServer() to initialize and start the configuration web server.
 *  - Call handleWebServer() regularly in your main loop to process HTTP requests.
 *  - Use saveConfigToEEPROM() to persist changes made via the web interface or programmatically.
 *  - Use registerCustomEndpoint() to add additional HTTP endpoints to the configuration server.
 *
 * Any changes to this file directly affect the configuration logic and web interface
 * of the OTA Template project.
 *
 * Author: R. Zuehlsdorff
 * Copyright 2025
 */

#include <EEPROM.h>
#include "OTA_WebConfig.h"
#include "OTA_WebForm.h"  // HTML form for the web interface



// Change from Config to OTAConfig
OTAConfig config; // Global configuration structure
const OTAConfig *defaults; // Pointer to default configuration structure

#if defined(ESP8266)
  ESP8266WebServer server(config.webServerPort);
#elif defined(ESP32)
  WebServer server(config.webServerPort);
#endif


// Call this function in setup() to check if OTAConfig struct fits in EEPROM
void checkConfigSize() {
    if (sizeof(OTAConfig) > EEPROM_SIZE) {
        Serial.println("WARNING: OTAConfig struct size exceeds EEPROM_SIZE! Data may be lost.");
    }
}

/**
 * handleRoot()
 * Called when the root page ("/") is opened in the browser.
 * Sends the HTML configuration form to the client.
 */
void handleRoot() {
  server.send(200, "text/html", htmlForm());
}

/**
 * handleSet()
 * Called when the configuration form is submitted (POST to "/set").
 * Reads the form data, stores it in the OTAConfig structure, and writes it to EEPROM.
 * Detects if a restart is requested and restarts the device if necessary.
 */
void handleSet() {
  // Check for reset to defaults
  if (server.hasArg("resetDefaults") && server.arg("resetDefaults") == "1") {
    // Set all config fields to defaults
    setDefaultConfig(config, defaults);
    // Save defaults to EEPROM using the helper function
    saveConfigToEEPROM();

    // Redisplay the form with default values
    server.send(200, "text/html", htmlForm());
    return;
  }

  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String otaServer = server.arg("otaServer");
  String otaPort = server.arg("otaPort");
  String otaEnabled = server.arg("otaEnabled");
  String otaUpdateInterval = server.arg("otaUpdateInterval");
  String webServerPort = server.arg("webServerPort");

  // Copy values into the OTAConfig structure
  ssid.toCharArray(config.ssid, sizeof(config.ssid));
  password.toCharArray(config.password, sizeof(config.password));
  otaServer.toCharArray(config.otaServer, sizeof(config.otaServer));
  config.otaPort = otaPort.toInt();
  config.otaEnabled = (otaEnabled == "1");
  config.otaUpdateInterval = otaUpdateInterval.toInt();
  config.webServerPort = webServerPort.toInt();

  // Write configuration to EEPROM
  saveConfigToEEPROM();

  Serial.println("Configuration saved to EEPROM.");

  // Check if a restart is requested
  if (server.hasArg("restart") && server.arg("restart") == "1") {
    server.send(200, "text/plain", "Configuration saved. Restarting...");
    delay(500);
    ESP.restart();
    return;
  }

  server.send(200, "text/plain", "Configuration saved. Restart the device.");
}

/**
 * setDefaultConfig()
 * Sets the provided OTAConfig struct (cfg) to the values from the given default OTAConfig struct (defaults).
 */
void setDefaultConfig(OTAConfig &cfg, const OTAConfig *defaults) {
    if (!defaults) return;
    strcpy(cfg.ssid, defaults->ssid);
    strcpy(cfg.password, defaults->password);
    strcpy(cfg.otaServer, defaults->otaServer);
    cfg.otaPort = defaults->otaPort;
    cfg.otaEnabled = defaults->otaEnabled;
    cfg.otaUpdateInterval = defaults->otaUpdateInterval;
    strcpy(cfg.firmware_vers, defaults->firmware_vers);
    cfg.webServerPort = defaults->webServerPort;
    strcpy(cfg.appname, defaults->appname);
    strcpy(cfg.firmware_name, defaults->firmware_name);
    strcpy(cfg.description, defaults->description);
}

/**
 * loadConfig()
 * Loads the configuration from EEPROM into the global OTAConfig structure.
 * If no valid data is present, default values are set.
 * Prints the loaded values to the serial interface.
 */
void loadConfig(const OTAConfig *default_config) {
  defaults = default_config; // Set the defaults pointer to the provided default config
  // Read the configuration from EEPROM
  config = readConfigFromEEPROM();

  // Check if the SSID is valid (simple check)
  if (config.ssid[0] == '\0' || config.ssid[0] == 0xFF) {
    setDefaultConfig(config, defaults);
    Serial.println("EEPROM empty, loaded default values.");
  } else {
    Serial.println("Configuration loaded from EEPROM.");
  }

  Serial.printf("SSID: %s\n", config.ssid);
  Serial.printf("Password: %s\n", config.password);
  Serial.printf("OTA Server: %s\n", config.otaServer);
  Serial.printf("OTA Port: %d\n", config.otaPort);
  Serial.printf("OTA Enabled: %s\n", config.otaEnabled ? "true" : "false");
  Serial.printf("Firmware Version: %s\n", config.firmware_vers);
  Serial.printf("App Name: %s\n", config.appname);
  Serial.printf("Firmware Name: %s\n", config.firmware_name);
  Serial.printf("Description: %s\n", config.description);
}

/**
 * startWebServer()
 * Initializes the web server, registers the handlers for the root page and setting the configuration.
 * Starts the web server.
 */
void startWebServer() {
  server.on(OTA_CONFIG_ROOT, handleRoot); // Use OTA_CONFIG_ROOT for the root page
  server.on(OTA_CONFIG_SET, HTTP_POST, handleSet); // Use OTA_CONFIG_SET for the config set endpoint
  server.begin();
  Serial.println("Web server started.");
}

/**
 * handleWebServer()
 * Must be called regularly in the loop() function.
 * Handles incoming HTTP requests.
 */
void handleWebServer() {
  server.handleClient();
}

/**
 * saveConfigToEEPROM()
 * Saves the current configuration to EEPROM.
 */
void saveConfigToEEPROM() {
#if defined(ESP8266)
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(EEPROM_START, config);
  EEPROM.commit();
#elif defined(ESP32)
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM for ESP32");
    return;
  }
  EEPROM.put(EEPROM_START, config);
  EEPROM.commit();
  EEPROM.end();
#endif
}

/**
 * readConfigFromEEPROM()
 * Reads the configuration from EEPROM and returns it.
 */
OTAConfig readConfigFromEEPROM() {
  OTAConfig cfg;
#if defined(ESP8266)
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_START, cfg);
#elif defined(ESP32)
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM for ESP32");
    memset(&cfg, 0, sizeof(cfg));
    return cfg;
  }
  EEPROM.get(EEPROM_START, cfg);
  EEPROM.end();
#endif
  return cfg;
}

/**
 * registerCustomEndpoint
 * Registers an additional endpoint and its handler function with the web server.
 * 
 * @param uri The URI path for the endpoint (e.g. "/custom")
 * @param handler The function to handle requests to this endpoint
 * @param method (optional) HTTP method (default: HTTP_GET)
 *
 * Example usage:
 *   registerCustomEndpoint("/custom", []() { server.send(200, "text/plain", "Hello from custom endpoint!"); });
 */
void registerCustomEndpoint(const String& uri, std::function<void(void)> handler, HTTPMethod method) {
    server.on(uri.c_str(), method, handler);
}