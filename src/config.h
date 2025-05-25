#ifndef CONFIG_H
#define CONFIG_H

#define APSSID "RZHome"       // Default WiFi SSID
#define APPSK "7895350500384967"    // Default WiFi Password

#define OTA_SERVER "192.168.1.100" // Replace with your OTA server's IP address
#define OTA_PORT 3000
#define UPDATE_PATH "/updates/"
#define FIRMWARE_NAME "firmware.bin"
#define VERSION_FILE "firmware.bin.version" // Align with ota-server.js
#define CURRENT_VERSION "1.0.0"

#endif // CONFIG_H
