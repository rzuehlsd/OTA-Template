/**
 * OTA_WebConfig.h
 *
 * This header defines the OTAConfig structure and declares all functions and global variables
 * required for managing, saving, and providing configuration data for the OTA Template project.
 *
 * It includes:
 *   - The global configuration structure (OTAConfig)
 *   - Functions for loading and saving configuration in EEPROM
 *   - Initialization and operation of the web server for configuration
 *   - Handlers for web server endpoints (Root, Set, etc.)
 *   - Registration of custom web endpoints
 *
 * Any changes to this file directly affect the configuration and web interface logic
 * of the OTA Template project.
 *
 * Author: R. Zuehlsdorff
 * Copyright 2025
 */

#ifndef OTA_WEBCONFIG_H
#define OTA_WEBCONFIG_H

#if defined(ESP8266)
  #include <ESP8266WebServer.h>
  #include <functional>
  typedef ESP8266WebServer WebConfigServer;
#elif defined(ESP32)
  #include <WebServer.h>
  #include <functional>
  typedef WebServer WebConfigServer;
#endif

// Static configurations and constants used
#define OTA_CONFIG_VERSION "1.0.0"      // Version of the OTA configuration system
#define OTA_CONFIG_ROOT "/ota"          // Root path for OTA updates on the ota-server
#define OTA_CONFIG_SET "/ota/set"       // Path for setting OTA configuration via web interface
#define EEPROM_SIZE 256                 // Size of the EEPROM region used for storing configuration
#define EEPROM_START 0                  // Start address in EEPROM for storing configuration data

struct OTAConfig {
  char ssid[32];               // WiFi SSID for network connection
  char password[32];           // WiFi password for network connection
  char otaServer[32];          // OTA server IP address or hostname
  int otaPort;                 // OTA server port number
  bool otaEnabled;             // Enable (true) or disable (false) OTA updates
  unsigned long otaUpdateInterval; // Interval (in minutes) to check for OTA updates
  int webServerPort;           // Port number for the device's web configuration server
  char appname[32];            // Application name shown in the web interface
  char firmware_name[32];      // Firmware binary file name on the OTA server
  char firmware_vers[16];      // Current firmware version string
  char description[256];       // Description of the device/application for the web interface
};

typedef struct OTAConfig OTAConfig;


// Global configuration instance, holds all current settings
extern OTAConfig config;

// Global web server instance for handling configuration requests
extern WebConfigServer server;


// Configuration management functions (used in OTA_Template.cpp)
/**
 * Checks if the OTAConfig struct fits into the defined EEPROM_SIZE.
 */
void checkConfigSize();

/**
 * Saves the current configuration to EEPROM.
 */
void saveConfigToEEPROM();

/**
 * Reads the configuration from EEPROM and returns it as an OTAConfig struct.
 */
OTAConfig readConfigFromEEPROM();

/**
 * Loads configuration from EEPROM into the global config variable.
 * If not present, uses the provided default configuration.
 */
void loadConfig(const OTAConfig *default_config);

/**
 * Sets the provided OTAConfig struct (cfg) to the values from the given default OTAConfig struct (defaults).
 */
void setDefaultConfig(OTAConfig &cfg, const OTAConfig *defaults);


// Web server and config handlers (used in OTA_Template.cpp)
/**
 * Handles the root endpoint (main configuration page).
 */
void handleRoot();

/**
 * Handles the configuration form submission.
 */
void handleSet();

/**
 * Starts the web server for configuration interface.
 */
void startWebServer();

/**
 * Handles incoming web server requests.
 */
void handleWebServer();

/**
 * Registers a custom web endpoint for the configuration server.
 */
void registerCustomEndpoint(const String& uri, std::function<void(void)> handler, HTTPMethod method);

#endif // OTA_WEBCONFIG_H
