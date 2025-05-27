/**
 * OTA_Template.cpp
 * 
 * Dieses File enthält die zentralen Funktionen für:
 *  - Automatische OTA-Firmware-Updates für ESP8266
 *  - Verwaltung und Überwachung der WLAN-Verbindung
 *  - Integration und Ablauf der webbasierten Konfigurationsoberfläche
 * 
 * Verwendung:
 * ----------------------------------------------------------------------------
 * - otaSetup() im setup() aufrufen, um Konfiguration, WLAN und Webserver zu initialisieren.
 * - otaLoop() im loop() aufrufen, um OTA-Logik und Webserver-Anfragen zu bearbeiten.
 * 
 * Konfigurations-Handling:
 * ----------------------------------------------------------------------------
 * Beim Start werden Konfigurationsdaten (WLAN, OTA-Server, Update-Intervall etc.) 
 * aus dem EEPROM geladen (loadConfig()). Falls keine gültigen Daten gefunden werden, 
 * werden Default-Werte aus config.h verwendet.
 * 
 * Änderungen über die Weboberfläche werden im EEPROM gespeichert und nach Neustart wirksam.
 * Die Weboberfläche erlaubt komfortables Bearbeiten und Speichern aller relevanten Parameter.
 * 
 * Enthaltene Funktionen:
 *  - ensureWiFiConnection(): Stellt sicher, dass WLAN verbunden ist.
 *  - indicateUpdateStatus(): Zeigt OTA-Update-Status per LED an.
 *  - performOTAUpdate(): Prüft und führt Firmware-Updates durch.
 *  - otaSetup(): Initialisiert Konfiguration, WLAN und Webserver.
 *  - otaLoop(): Bearbeitet OTA-Logik und Webserver-Anfragen.
 */

#include <Arduino.h>
#include <vector>
#include <sstream>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <EEPROM.h>
#include "config.h"
#include "WebConfig.h"


extern Config config;
WiFiClient client;

/**
 * Stellt sicher, dass das Gerät mit dem WLAN verbunden ist.
 * Baut ggf. eine Verbindung auf und gibt die IP-Adresse aus.
 */
void ensureWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

/**
 * Zerlegt einen Versionsstring (z.B. "1.2.3") in Integer-Komponenten.
 * Liefert einen Vektor mit den einzelnen Zahlen zurück.
 */
std::vector<int> splitVersion(const String& version) {
  std::vector<int> parts;
  int start = 0, end = 0;
  while ((end = version.indexOf('.', start)) != -1) {
    parts.push_back(version.substring(start, end).toInt());
    start = end + 1;
  }
  parts.push_back(version.substring(start).toInt());
  return parts;
}

/**
 * Vergleicht zwei Versionsstrings.
 * Rückgabewert: -1 wenn v1 < v2, 1 wenn v1 > v2, 0 wenn gleich.
 */
int compareVersion(const String& v1, const String& v2) {
  std::vector<int> ver1 = splitVersion(v1);
  std::vector<int> ver2 = splitVersion(v2);
  size_t len = std::max(ver1.size(), ver2.size());
  for (size_t i = 0; i < len; ++i) {
    int num1 = (i < ver1.size()) ? ver1[i] : 0;
    int num2 = (i < ver2.size()) ? ver2[i] : 0;
    if (num1 < num2) return -1;
    if (num1 > num2) return 1;
  }
  return 0;
}

/**
 * Zeigt den Status des OTA-Updates über die LED und die serielle Schnittstelle an.
 * - Bei Fehler: LED dauerhaft an
 * - Kein Update: LED aus
 * - Erfolgreiches Update: LED blinkt 5x
 */
void indicateUpdateStatus(t_httpUpdate_return ret, String vers) {
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      digitalWrite(LED_BUILTIN, HIGH); // Error: LED stays on
      Serial.println("OTA Update failed!");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      digitalWrite(LED_BUILTIN, LOW); // No updates: LED off
      Serial.println("No OTA Update available!");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("OTA Update to version " + vers +  " completed!");
      for (int i = 0; i < 5; i++) { // Success: LED blinks 5 times
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
      }
      break;
  }
}

/**
 * Prüft, ob eine neue Firmware-Version auf dem OTA-Server verfügbar ist,
 * und führt ggf. das Update durch. Speichert die neue Version im EEPROM.
 */
void performOTAUpdate() {
  String newVersion;
  int comp = -1;
  char path[128];
  char buf[128];
  sprintf(path, "http://%s:%d/updates/%s", config.otaServer, config.otaPort, FIRMWARE_NAME);
  sprintf(buf, "http://%s:%d/version/%s.version", config.otaServer, config.otaPort, FIRMWARE_NAME);

  Serial.printf("Starting OTA update from: %s\n", path);
  Serial.printf("Checking firmware version from: %s\n", buf);

  HTTPClient http;
  if (http.begin(client, buf)) {
    int httpCode = http.GET();
    Serial.printf("HTTP response code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      newVersion = http.getString();
      newVersion.trim();
      comp = compareVersion(newVersion, config.version);
      Serial.printf("Available firmware version: %s\n", newVersion.c_str());
      if (comp == 0){
        Serial.println("Firmware is already up-to-date.");
        http.end();
        return;
      }
    } else {
      Serial.printf("Failed to check firmware version, HTTP code: %d\n", httpCode);
    }
    http.end();
  } else {
    Serial.println("Failed to connect to version check URL.");
  }

  if(comp > 0) {  // There is a new version on OTA server available
    Serial.printf("New firmware version %s available, current version is %s\n", newVersion.c_str(), config.version);
    strcpy(config.version, newVersion.c_str());
    saveConfigToEEPROM(); // Save new version to EEPROM
    Serial.println("EEPROM Version updated -> Starting OTA update...");
    unsigned long startTime = millis();
    while (millis() - startTime < 30000) {
      t_httpUpdate_return ret = ESPhttpUpdate.update(client, path);
      indicateUpdateStatus(ret, newVersion);
      if (ret == HTTP_UPDATE_OK) {
        break;
      }
    }
  }
}

/**
 * Initialisiert die Konfiguration, verbindet mit WLAN und startet den Webserver.
 */
void otaSetup() {
  loadConfig(); // Load configuration from EEPROM
  Serial.println("READY - Connecting to WiFi ..");

  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.password);

  ensureWiFiConnection();

  Serial.print(F("Firmware version "));
  Serial.println(config.version);

  startWebServer(); // Start web configuration
}

/**
 * Führt zyklisch die OTA-Logik aus, prüft auf Updates und bearbeitet Webserver-Anfragen.
 */
void otaLoop() {
  ensureWiFiConnection();
  handleWebServer(); // Handle web server requests
  if(config.otaEnabled) {
    // Check for OTA updates every configured interval
    static unsigned long lastUpdateCheck = 0;
    if (millis() - lastUpdateCheck > config.otaUpdateInterval * 60000) { // Convert minutes to milliseconds
      performOTAUpdate();
      lastUpdateCheck = millis();
    }
  }
}