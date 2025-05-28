/**
 * OTA_WebConfig.h
 *
 * This header file declares the central configuration structure (Config)
 * as well as all functions for managing, saving, and providing
 * configuration data for the OTA Template project.
 *
 * Contains declarations for:
 *   - The global configuration structure (Config)
 *   - Loading and saving the configuration in EEPROM
 *   - Initialization and operation of the web server for configuration
 *   - Handlers for web server endpoints (Root, Set, etc.)
 *
 * Changes to this file directly affect the entire configuration logic.
 */

#ifndef OTA_WEBCONFIG_H
#define OTA_WEBCONFIG_H

#include <ESP8266WebServer.h>

// Structure to hold configuration
// Configuration structure including OTA enable flag
// OTA_UPDATE_INTERVAL is included as a parameter in the struct
struct Config {
  char ssid[32];              // WiFi SSID for network connection
  char password[32];          // WiFi password for network connection
  char otaServer[32];         // OTA server address (IP or hostname)
  int otaPort;                // OTA server port
  bool otaEnabled;            // Flag: true if OTA updates are enabled
  unsigned long otaUpdateInterval; // OTA update interval in minutes
  char version[16];           // Current firmware version string
  int webServerPort;          // Port number for the web server to serve the configuration page
  // Add more fields as needed
};

extern Config config; // Declare the config variable

extern ESP8266WebServer server;  // Web server instance for handling configuration requests

// Checks if the Config struct fits in EEPROM
void checkConfigSize();

// Saves the current configuration to EEPROM
void saveConfigToEEPROM();

// Reads configuration from EEPROM and returns it as a Config struct
Config readConfigFromEEPROM();

// Loads configuration from EEPROM or sets defaults if not available
void loadConfig();

// Starts the web server for configuration interface
void startWebServer();

// Handles incoming web server requests
void handleWebServer();

// Handles the root endpoint (main configuration page)
void handleRoot();

// register customer web pages as an extension
void registerCustomEndpoint(const String& uri, std::function<void(void)> handler, HTTPMethod method);



#endif // OTA_WEBCONFIG_H
