/**
 * WebConfig.h
 *
 * Dieses Header-File deklariert die zentrale Konfigurationsstruktur (Config)
 * sowie alle Funktionen zur Verwaltung, Speicherung und Bereitstellung der
 * Konfigurationsdaten für das OTA Template Projekt.
 *
 * Enthält Deklarationen für:
 *   - Die globale Konfigurationsstruktur (Config)
 *   - Laden und Speichern der Konfiguration im EEPROM
 *   - Initialisierung und Betrieb des Webservers zur Konfiguration
 *   - Handler für Webserver-Endpunkte (Root, Set, etc.)
 *
 * Änderungen an diesem File wirken sich auf die gesamte Konfigurationslogik aus.
 */

#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <ESP8266WebServer.h>

// Structure to hold configuration
// Konfigurationsstruktur inkl. OTA-Enable-Flag
// Add OTA_UPDATE_INTERVAL as a parameter in the struct
struct Config {
  char ssid[32];
  char password[32];
  char otaServer[32];
  int otaPort;
  bool otaEnabled;
  unsigned long otaUpdateInterval; // in milliseconds
  char version[16]; // Firmware version
  // Add more fields as needed
};

extern Config config; // Declare the config variable

void loadConfig();
void startWebServer();
void handleWebServer();
void handleRoot(); // Declare handleRoot function
void saveConfigToEEPROM();
Config readConfigFromEEPROM();

#endif // WEBCONFIG_H
