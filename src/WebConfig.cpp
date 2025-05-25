#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "WebConfig.h"
#include "config.h"
#include "WebForm.h"  // <-- WebForm einbinden

ESP8266WebServer server(80);
Config config; // Use the declaration from the header file

// Handle root page
void handleRoot() {
  server.send(200, "text/html", htmlForm());
}

// Handle form submission
void handleSet() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String otaServer = server.arg("otaServer");
  String otaPort = server.arg("otaPort");
  String otaEnabled = server.arg("otaEnabled");
  String otaUpdateInterval = server.arg("otaUpdateInterval");

  // Save values to config struct
  ssid.toCharArray(config.ssid, sizeof(config.ssid));
  password.toCharArray(config.password, sizeof(config.password));
  otaServer.toCharArray(config.otaServer, sizeof(config.otaServer));
  config.otaPort = otaPort.toInt();
  config.otaEnabled = (otaEnabled == "1");
  config.otaUpdateInterval = otaUpdateInterval.toInt();

  // Write config to EEPROM
  EEPROM.begin(EEPROM_START);
  EEPROM.put(0, config);
  EEPROM.commit();

  Serial.println("Configuration saved to EEPROM.");
  server.send(200, "text/plain", "Configuration saved. Restart the device.");
}

void loadConfig() {
  EEPROM.begin(EEPROM_START);
  EEPROM.get(0, config);

  // Prüfen, ob die SSID gültig ist (einfacher Check)
  if (config.ssid[0] == '\0' || config.ssid[0] == 0xFF) {
    // Wenn EEPROM leer oder ungültig, Standardwerte setzen
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    config.otaEnabled = OTA_ENABLED; 
    config.otaUpdateInterval = OTA_UPDATE_INTERVAL;
    Serial.println("EEPROM leer, Standardwerte geladen.");
  } else {
    Serial.println("Konfiguration aus EEPROM geladen.");
  }

  Serial.printf("SSID: %s\n", config.ssid);
  Serial.printf("Password: %s\n", config.password);
  Serial.printf("OTA Server: %s\n", config.otaServer);
  Serial.printf("OTA Port: %d\n", config.otaPort);
  Serial.printf("OTA Enabled: %s\n", config.otaEnabled ? "true" : "false");
}

void startWebServer() {
  server.on("/", handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("Web server started.");
}

void handleWebServer() {
  server.handleClient();
  if (server.hasArg("restart") && server.arg("restart") == "1") {
    server.send(200, "text/plain", "Configuration saved. Restarting...");
    delay(500);
    ESP.restart();
    return;
  }
}
