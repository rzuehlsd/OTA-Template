#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <ESP8266WebServer.h>

// Structure to hold configuration
struct Config {
  char ssid[32];
  char password[32];
  char otaServer[64];
  int otaPort;
  char updatePath[64];
};

extern Config config; // Declare the config variable

void loadConfig();
void startWebServer();
void handleWebServer();
void handleRoot(); // Declare handleRoot function

#endif // WEBCONFIG_H
