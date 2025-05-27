/**
 * WebConfig.cpp
 * 
 * This file provides all functions for handling configuration data,
 * web server endpoints, and EEPROM storage for ESP8266 projects.
 * It enables configuration of WiFi, OTA server, and update interval
 * via a web interface and ensures persistent storage in EEPROM.
 * 
 * How to extend for user-defined configuration:
 * ----------------------------------------------------------------------------
 * 1. Extend the Config struct (in config.h) with your own variables.
 * 2. Add new input fields to the HTML form in WebForm.h and handle them in handleSet():
 *    - Read the new value from the web form using server.arg("yourField").
 *    - Store it in the config struct.
 * 3. Update loadConfig() to set default values for your new fields if needed.
 * 4. Display your new configuration values in the web form as required.
 * 
 * Example: To add a "deviceName" field:
 *   - Add `char deviceName[32];` to Config in config.h.
 *   - Add an input field for "deviceName" in WebForm.h.
 *   - In handleSet(), add: server.arg("deviceName").toCharArray(config.deviceName, sizeof(config.deviceName));
 *   - In loadConfig(), set a default if EEPROM is empty.
 *   - Show the value in the HTML form.
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "WebConfig.h"
#include "config.h"
#include "WebForm.h"  // HTML-Formular für die Weboberfläche

ESP8266WebServer server(80);
Config config; // Globale Konfigurationsstruktur

/**
 * handleRoot()
 * Wird aufgerufen, wenn die Root-Seite ("/") im Browser geöffnet wird.
 * Sendet das HTML-Konfigurationsformular an den Client.
 */
void handleRoot() {
  server.send(200, "text/html", htmlForm());
}

/**
 * handleSet()
 * Wird aufgerufen, wenn das Konfigurationsformular abgeschickt wird (POST auf "/set").
 * Liest die Formulardaten aus, speichert sie in der Config-Struktur und schreibt sie ins EEPROM.
 * Erkennt, ob ein Neustart gewünscht ist, und startet ggf. das Gerät neu.
 */
void handleSet() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String otaServer = server.arg("otaServer");
  String otaPort = server.arg("otaPort");
  String otaEnabled = server.arg("otaEnabled");
  String otaUpdateInterval = server.arg("otaUpdateInterval");

  // Werte in die Config-Struktur übernehmen
  ssid.toCharArray(config.ssid, sizeof(config.ssid));
  password.toCharArray(config.password, sizeof(config.password));
  otaServer.toCharArray(config.otaServer, sizeof(config.otaServer));
  config.otaPort = otaPort.toInt();
  config.otaEnabled = (otaEnabled == "1");
  config.otaUpdateInterval = otaUpdateInterval.toInt();

  // Konfiguration ins EEPROM schreiben
  EEPROM.begin(EEPROM_START);
  EEPROM.put(0, config);
  EEPROM.commit();

  Serial.println("Configuration saved to EEPROM.");

  // Prüfen, ob ein Neustart gewünscht ist
  if (server.hasArg("restart") && server.arg("restart") == "1") {
    server.send(200, "text/plain", "Configuration saved. Restarting...");
    delay(500);
    ESP.restart();
    return;
  }

  server.send(200, "text/plain", "Configuration saved. Restart the device.");
}

/**
 * loadConfig()
 * Lädt die Konfiguration aus dem EEPROM in die globale Config-Struktur.
 * Falls keine gültigen Daten vorhanden sind, werden Standardwerte gesetzt.
 * Gibt die geladenen Werte auf der seriellen Schnittstelle aus.
 */
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

/**
 * startWebServer()
 * Initialisiert den Webserver, registriert die Handler für die Root-Seite und das Setzen der Konfiguration.
 * Startet den Webserver.
 */
void startWebServer() {
  server.on("/", handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("Web server started.");
}

/**
 * handleWebServer()
 * Muss regelmäßig in der loop()-Funktion aufgerufen werden.
 * Bearbeitet eingehende HTTP-Anfragen.
 */
void handleWebServer() {
  server.handleClient();
}
