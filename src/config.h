/**
 * config.h
 *
 * Central configuration file for the OTA Template project.
 * Contains all global constants, default values, and macros for
 * WiFi, OTA, and firmware settings.
 *
 * Here you define the default SSID, WiFi password, OTA server address,
 * firmware version, EEPROM size, and other global parameters.
 * Changes to these values affect the entire project.
 */

#ifndef CONFIG_H
#define CONFIG_H


/*
**  WiFi Configuration Details  
*/
#define APSSID "RZHome"                // Default WiFi SSID used for initial connection or reset
#define APPSK "7895350500384967"       // Default WiFi password used for initial connection or reset

/*
**  OTA Server Configuration Details  
*/
#define OTA_SERVER "192.168.2.198"     // OTA server IP or hostname for firmware updates
#define OTA_VERSION "1.0.0.0"          // Version number of the OTA template (not firmware)
#define OTA_PORT 3000                  // Port number used to connect to the OTA server
#define OTA_UPDATE_INTERVAL 60         // Interval (in minutes) to check for OTA updates
#define OTA_ENABLED false              // Enable (true) or disable (false) OTA update functionality by default
#define OTA_CONFIG_ROOT "/ota"         // Root path for OTA updates on the ota-#server
#define OTA_CONFIG_SET "/ota/set"      // Path for setting OTA configuration via web interface

/*
**  Application Configuration Details  
*/
#define APPNAME "OTA_Template Test"    // Application name shown in the web interface
#define FIRMWARE_NAME "ota_test_app.bin"   // Firmware binary file name on the OTA server
#define FIRMWARE_VERSION  "1.1.0"      // Current firmware version of this build
#define WEB_SERVER_PORT 80             // Port number for the web server to serve the configuration page
#define DESCRIPTION   u8"Sample program to demonstrate the use of the OTA Update Template\n"\
                        "with Web Configuration on ESP8266, ESP32s3, ESP32c3\n"\ 
                        "User pages at //hello "

#endif // CONFIG_H
