/**
 * config.h
 *
 * Zentrale Konfigurationsdatei für das OTA Template Projekt.
 * Enthält alle globalen Konstanten, Default-Werte und Makros für die
 * WLAN-, OTA- und Firmware-Einstellungen.
 *
 * Hier werden u.a. der Standard-SSID, das WLAN-Passwort, OTA-Server-Adresse,
 * Firmware-Version, EEPROM-Größe und weitere globale Parameter definiert.
 * Änderungen an diesen Werten wirken sich auf das gesamte Projekt aus.
 */

#ifndef CONFIG_H
#define CONFIG_H

#define APPNAME "OTA_Template Test"

#define APSSID "RZHome"       // Default WiFi SSID
#define APPSK "7895350500384967"    // Default WiFi Password

#define OTA_SERVER "192.168.2.198" // Replace with your remote OTA server's IP address
#define OTA_PORT 3000
#define FIRMWARE_NAME "firmware.bin"
#define INITIAL_VERSION  "1.1.0"
#define OTA_UPDATE_INTERVAL 1 // 10 minutes
#define EEPROM_SIZE 256
#define EEPROM_START 0
#define OTA_ENABLED false
#define DESCRIPTION u8"OTA Update Template mit Web Configuration für ESP8266"

#endif // CONFIG_H
