/**
 * WebConfig.cpp
 *
 * Dieses File enthält alle Funktionen zur Verwaltung der Konfigurationsdaten,
 * zur Bereitstellung der Webserver-Endpunkte und zur Speicherung/Ladung der
 * Einstellungen im EEPROM für ESP8266-Projekte.
 *
 * Über die Weboberfläche können WLAN-, OTA- und weitere Einstellungen
 * komfortabel geändert und dauerhaft gespeichert werden.
 *
 * Hinweise zur Erweiterung für eigene Konfigurationsfelder:
 * ----------------------------------------------------------------------------
 * 1. Die Config-Struktur (in config.h) um eigene Variablen erweitern.
 * 2. Neue Eingabefelder im HTML-Formular in WebForm.h ergänzen und in handleSet()
 *    auslesen und in die Config-Struktur übernehmen.
 * 3. In loadConfig() ggf. Standardwerte für neue Felder setzen.
 * 4. Die neuen Werte im Formular anzeigen.
 *
 * Beispiel: Für ein neues Feld "deviceName":
 *   - In config.h: char deviceName[32]; zur Config-Struktur hinzufügen.
 *   - In WebForm.h: Eingabefeld für "deviceName" ergänzen.
 *   - In handleSet(): server.arg("deviceName").toCharArray(config.deviceName, sizeof(config.deviceName));
 *   - In loadConfig(): Standardwert setzen, falls EEPROM leer.
 *   - Im Formular anzeigen.
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
  // Check for reset to defaults
  if (server.hasArg("resetDefaults") && server.arg("resetDefaults") == "1") {
    // Set all config fields to defaults
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    config.otaEnabled = OTA_ENABLED;
    config.otaUpdateInterval = OTA_UPDATE_INTERVAL;
    strcpy(config.version, INITIAL_VERSION); // Make sure INITIAL_VERSION is defined

    // Save defaults to EEPROM using the helper function
    saveConfigToEEPROM();

    // Redisplay the form with default values
    server.send(200, "text/html", htmlForm());
    return;
  }

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
  saveConfigToEEPROM();

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
 * readConfigFromEEPROM()
 * Liest die Konfiguration aus dem EEPROM und gibt sie zurück.
 */
Config readConfigFromEEPROM() {
  Config cfg;
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_START, cfg);
  return cfg;
}

/**
 * loadConfig()
 * Lädt die Konfiguration aus dem EEPROM in die globale Config-Struktur.
 * Falls keine gültigen Daten vorhanden sind, werden Standardwerte gesetzt.
 * Gibt die geladenen Werte auf der seriellen Schnittstelle aus.
 */
void loadConfig() {
  config = readConfigFromEEPROM();

  // Prüfen, ob die SSID gültig ist (einfacher Check)
  if (config.ssid[0] == '\0' || config.ssid[0] == 0xFF) {
    // Wenn EEPROM leer oder ungültig, Standardwerte setzen
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    config.otaEnabled = OTA_ENABLED; 
    config.otaUpdateInterval = OTA_UPDATE_INTERVAL;
    strcpy(config.version, INITIAL_VERSION);
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

void saveConfigToEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(EEPROM_START, config); // Use macro as the start address
  EEPROM.commit();
}

