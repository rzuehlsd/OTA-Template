/* ESP8266 Basic Sketch for automatic update of firmware at start */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <EEPROM.h>
#include "config.h"
#include "WebConfig.h"

#define DEBUG true

#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif

WiFiClient client;

char path[128];
char buf[128];

/**
 * Ensures the ESP8266 is connected to WiFi.
 * Retries until a connection is established.
 */
void ensureWiFiConnection() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("WiFi connected!");
}

/**
 * Indicates the status of the OTA update using the built-in LED.
 * - LED on: Update failed
 * - LED off: No updates available
 * - LED blinks: Update successful
 */
void indicateUpdateStatus(t_httpUpdate_return ret) {
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      digitalWrite(LED_BUILTIN, HIGH); // Fehler: LED dauerhaft an
      break;
    case HTTP_UPDATE_NO_UPDATES:
      digitalWrite(LED_BUILTIN, LOW); // Keine Updates: LED aus
      break;
    case HTTP_UPDATE_OK:
      for (int i = 0; i < 5; i++) { // Erfolg: LED blinkt 5-mal
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
      }
      break;
  }
}

/**
 * Performs an OTA update by connecting to the specified server.
 * Checks if the firmware version matches before updating.
 * Retries for up to 30 seconds if the update fails.
 */
void performOTAUpdate() {
  sprintf(path, "%s:%d%s%s", OTA_SERVER, OTA_PORT, UPDATE_PATH, FIRMWARE_NAME);
  Serial.printf("Starting OTA update from: %s\n", path);

  // Check firmware version before updating
  HTTPClient http;
  sprintf(buf, "%s:%d%s%s.version", OTA_SERVER, OTA_PORT, UPDATE_PATH, FIRMWARE_NAME);
  Serial.printf("Checking firmware version from: %s\n", buf);

  if (http.begin(client, buf)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String newVersion = http.getString();
      Serial.printf("Available firmware version: %s\n", newVersion.c_str());
      if (newVersion == FIRMWARE_VERSION) {
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

  unsigned long startTime = millis();
  while (millis() - startTime < 30000) { // 30 Sekunden Timeout
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, path);
    indicateUpdateStatus(ret);
    if (ret == HTTP_UPDATE_OK) {
      break;
    }
  }
}

/**
 * Sets up the ESP8266 by initializing serial communication,
 * loading configuration from EEPROM, connecting to WiFi,
 * starting the web server, and printing the firmware version.
 */
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("READY");

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  loadConfig(); // Load configuration from EEPROM

  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid, config.password);

  Serial.print(F("Firmware version "));
  Serial.println(FIRMWARE_VERSION);
  delay(2000);

  startWebServer(); // Start the web server
}

/**
 * Main loop that ensures WiFi connection, handles the web server,
 * and performs OTA updates.
 */
void loop() {
  ensureWiFiConnection();
  handleWebServer(); // Handle web server requests
  performOTAUpdate();
}
