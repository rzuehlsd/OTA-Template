/**
 * OTA_WebConfig.cpp
 *
 * This file contains all functions for managing configuration data,
 * providing web server endpoints, and saving/loading settings
 * in EEPROM for ESP8266/ESP32 projects.
 */

#include "OTA_WebConfig.h"
#include <EEPROM.h>
#include "OTA_WebForm.h"  // HTML form for the web interface

#if defined(ESP8266)
  ESP8266WebServer server(WEB_SERVER_PORT);
#elif defined(ESP32)
  WebServer server(WEB_SERVER_PORT);
#endif

Config config; // Global configuration structure

// Call this function in setup() to check if Config struct fits in EEPROM
void checkConfigSize() {
    if (sizeof(Config) > EEPROM_SIZE) {
        Serial.println("WARNING: Config struct size exceeds EEPROM_SIZE! Data may be lost.");
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
 * Reads the form data, stores it in the Config structure, and writes it to EEPROM.
 * Detects if a restart is requested and restarts the device if necessary.
 */
void handleSet() {
  // Check for reset to defaults
  if (server.hasArg("resetDefaults") && server.arg("resetDefaults") == "1") {
    // Set all config fields to defaults
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    config.otaEnabled = OTA_ENABLED;
    config.otaUpdateInterval = OTA_UPDATE_INTERVAL;
    strcpy(config.version, FIRMWARE_VERSION); // Make sure FIRMWARE_VERSION is defined
    config.webServerPort = WEB_SERVER_PORT;

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

  // Copy values into the Config structure
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
 * loadConfig()
 * Loads the configuration from EEPROM into the global Config structure.
 * If no valid data is present, default values are set.
 * Prints the loaded values to the serial interface.
 */
void loadConfig() {
  config = readConfigFromEEPROM();

  // Check if the SSID is valid (simple check)
  if (config.ssid[0] == '\0' || config.ssid[0] == 0xFF) {
    // If EEPROM is empty or invalid, set default values
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    config.otaEnabled = OTA_ENABLED; 
    config.otaUpdateInterval = OTA_UPDATE_INTERVAL;
    strcpy(config.version, FIRMWARE_VERSION);
    config.webServerPort = WEB_SERVER_PORT;
    Serial.println("EEPROM empty, loaded default values.");
  } else {
    Serial.println("Configuration loaded from EEPROM.");
  }

  Serial.printf("SSID: %s\n", config.ssid);
  Serial.printf("Password: %s\n", config.password);
  Serial.printf("OTA Server: %s\n", config.otaServer);
  Serial.printf("OTA Port: %d\n", config.otaPort);
  Serial.printf("OTA Enabled: %s\n", config.otaEnabled ? "true" : "false");
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
Config readConfigFromEEPROM() {
  Config cfg;
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