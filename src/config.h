#ifndef CONFIG_H
#define CONFIG_H

#define APPNAME "OTA_Template for ESP8266"

#define APSSID "RZHome"       // Default WiFi SSID
#define APPSK "7895350500384967"    // Default WiFi Password

#define OTA_SERVER "192.168.2.198" // Replace with your remote OTA server's IP address
#define OTA_PORT 3000
#define FIRMWARE_NAME "firmware.bin"
#define CURRENT_VERSION "1.0.0"
#define OTA_UPDATE_INTERVAL 10 // 10 minutes
#define EEPROM_START 256
#define OTA_ENABLED false


#endif // CONFIG_H
