#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "WebConfig.h"
#include "config.h"

ESP8266WebServer server(80);
Config config; // Use the declaration from the header file

// HTML form for setting variables
const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Configuration</title>
</head>
<body>
  <h1>Set Configuration</h1>
  <form action="/set" method="POST">
    <label for="ssid">WiFi SSID:</label><br>
    <input type="text" id="ssid" name="ssid" value=""><br><br>
    <label for="password">WiFi Password:</label><br>
    <input type="password" id="password" name="password" value=""><br><br>
    <label for="otaServer">OTA Server:</label><br>
    <input type="text" id="otaServer" name="otaServer" value=""><br><br>
    <label for="otaPort">OTA Port:</label><br>
    <input type="number" id="otaPort" name="otaPort" value=""><br><br>
    <label for="updatePath">Update Path:</label><br>
    <input type="text" id="updatePath" name="updatePath" value=""><br><br>
    <input type="submit" value="Save">
  </form>
</body>
</html>
)rawliteral";

// Handle root page
void handleRoot() {
  server.send(200, "text/html", htmlForm);
}

// Handle form submission
void handleSet() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String otaServer = server.arg("otaServer");
  String otaPort = server.arg("otaPort");
  String updatePath = server.arg("updatePath");

  // Save values to config struct
  ssid.toCharArray(config.ssid, sizeof(config.ssid));
  password.toCharArray(config.password, sizeof(config.password));
  otaServer.toCharArray(config.otaServer, sizeof(config.otaServer));
  config.otaPort = otaPort.toInt();
  updatePath.toCharArray(config.updatePath, sizeof(config.updatePath));

  // Write config to EEPROM
  EEPROM.begin(512);
  EEPROM.put(0, config);
  EEPROM.commit();

  Serial.println("Configuration saved to EEPROM.");
  server.send(200, "text/plain", "Configuration saved. Restart the device.");
}

void loadConfig() {
  EEPROM.begin(512);
  EEPROM.get(0, config);

  // If EEPROM is empty, set default values
  if (strlen(config.ssid) == 0) {
    strcpy(config.ssid, APSSID);
    strcpy(config.password, APPSK);
    strcpy(config.otaServer, OTA_SERVER);
    config.otaPort = OTA_PORT;
    strcpy(config.updatePath, UPDATE_PATH);
  }

  Serial.println("Configuration loaded from EEPROM:");
  Serial.printf("SSID: %s\n", config.ssid);
  Serial.printf("Password: %s\n", config.password);
  Serial.printf("OTA Server: %s\n", config.otaServer);
  Serial.printf("OTA Port: %d\n", config.otaPort);
  Serial.printf("Update Path: %s\n", config.updatePath);
}

void startWebServer() {
  server.on("/", handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("Web server started.");
}

void handleWebServer() {
  server.handleClient();
}
