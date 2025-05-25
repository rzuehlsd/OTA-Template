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
};

extern Config config; // Declare the config variable

void loadConfig();
void startWebServer();
void handleWebServer();
void handleRoot(); // Declare handleRoot function

#endif // WEBCONFIG_H
