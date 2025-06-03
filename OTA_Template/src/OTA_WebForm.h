/**
 * OTA_WebForm.h
 *
 * Provides the HTML form and related logic for the web-based configuration interface
 * of the OTA Template project. The htmlForm() function generates and returns the complete
 * HTML page as a string, including all input fields for WiFi, OTA server, firmware information,
 * and control buttons. The form reflects the current values from the global OTAConfig instance.
 *
 * Any changes to this file directly affect the device's web configuration interface.
 *
 * Author: R. Zuehlsdorff
 * Copyright 2025
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OTA_WEBFORM_H
#define OTA_WEBFORM_H

#include "OTA_WebConfig.h" // For OTAConfig definition


// Returns the HTML form as a String
inline String htmlForm() {
  String form = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>)rawliteral";
  form += String(config.appname); // Use appname as title
  form += R"rawliteral(</title>
  <style>
    body {
      background-color: #f0f0f0; /* light grey */
      font-family: Arial, sans-serif;
    }
    .form-frame {
      border: 3px solid #003366;
      border-radius: 10px;
      background: #e6f2ff; /* light blue */
      max-width: 500px;
      margin: 40px auto;
      padding: 24px 32px 16px 32px;
      box-shadow: 0 0 12px #b3c6e0;
    }
    table { border-collapse: collapse; width: 100%; }
    td, th { padding: 8px 12px; }
    td.label { text-align: right; font-weight: bold; }
    td.input { text-align: left; }
    input[type="text"], input[type="password"], input[type="number"], select {
      width: 100%;
      padding: 6px;
      box-sizing: border-box;
    }
    input[type="submit"], button[type="submit"] {
      background-color: #4CAF50;
      color: white;
      padding: 8px 16px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    input[type="submit"]:hover, button[type="submit"]:hover {
      background-color: #45a049;
    }
    button[type="submit"] {
      margin-left: 10px;
      background-color: #2196F3;
    }
    button[type="submit"]:hover {
      background-color: #1769aa;
    }
    button[type="button"].reset-btn {
      background-color: #f44336;
      color: white;
      margin-right: 10px;
    }
    button[type="button"].reset-btn:hover {
      background-color: #b71c1c;
    }
  </style>
  <script>
    function resetDefaults() {
      if(confirm('Reset all settings to default values?')) {
        var form = document.forms[0];
        var input = document.createElement('input');
        input.type = 'hidden';
        input.name = 'resetDefaults';
        input.value = '1';
        form.appendChild(input);
        form.submit();
      }
    }
  </script>
</head>
<body>
  <div class="form-frame">
    <h1 style="text-align:center;">)rawliteral";
  form += String(config.appname); // Use appname as main heading
  form += R"rawliteral(</h1>
    <h2 style="text-align:center; color:#003366; font-size:1.2em; margin-top:-10px; margin-bottom:24px;">)rawliteral";
  form += String(config.firmware_vers); // Firmware version as subtitle
  form += R"rawliteral(</h2>
    <div style="text-align:center; margin-bottom:20px;">
      <textarea readonly 
        style="width:100%;text-align:center;
               background:#fff;
               border:1px solid #bbb;
               color:#444;
               font-size:0.95em;
               font-family: inherit;
               padding:6px 8px;
               border-radius:6px;
               resize:none;"
        rows="3"
        >)rawliteral";
  form += String(config.description); // Use description
  form += R"rawliteral(</textarea>
    </div>
    <form action="/ota/set" method="POST">
      <table>
        <tr>
          <td class="label"><label for="ssid">WiFi SSID:</label></td>
          <td class="input"><input type="text" id="ssid" name="ssid" value=")rawliteral";
  form += String(config.ssid);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label"><label for="password">WiFi Key:</label></td>
          <td class="input"><input type="password" id="password" name="password" value=")rawliteral";
  form += String(config.password);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label"><label for="otaServer">OTA Server:</label></td>
          <td class="input"><input type="text" id="otaServer" name="otaServer" value=")rawliteral";
  form += String(config.otaServer);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label"><label for="otaPort">OTA Port:</label></td>
          <td class="input"><input type="number" id="otaPort" name="otaPort" value=")rawliteral";
  form += String(config.otaPort);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label"><label for="otaTemplateVersion">OTA Template Version:</label></td>
          <td class="input"><input type="text" id="otaTemplateVersion" name="otaTemplateVersion" value=")rawliteral";
  form += String(OTA_CONFIG_VERSION);
  form += R"rawliteral(" readonly></td>
        </tr>
        <tr>
          <td class="label"><label for="otaEnabled">OTA Service:</label></td>
          <td class="input">
            <select id="otaEnabled" name="otaEnabled">
              <option value="1")rawliteral";
  if (config.otaEnabled) form += " selected";
  form += R"rawliteral(>Enabled</option>
              <option value="0")rawliteral";
  if (!config.otaEnabled) form += " selected";
  form += R"rawliteral(>Disabled</option>
            </select>
          </td>
        </tr>
        <tr>
          <td class="label"><label for="otaUpdateInterval">OTA Update Interval (min):</label></td>
          <td class="input"><input type="number" id="otaUpdateInterval" name="otaUpdateInterval" min="1" value=")rawliteral";
  form += String(config.otaUpdateInterval);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label">Firmware Name:</td>
          <td class="input"><b>)rawliteral";
  form += String(config.firmware_name); // Use config.ssid as firmware name (or another attribute if you have one)
  form += R"rawliteral(</b></td>
        </tr>
        <tr>
          <td class="label">Firmware Version:</td>
          <td class="input"><b>)rawliteral";
  form += String(config.firmware_vers);
  form += R"rawliteral(</b></td>
        </tr>
        <tr>
          <td class="label">Web Server IP:</td>
          <td class="input"><b id="webServerIp"></b></td>
        </tr>
        <tr>
          <td class="label"><label for="webServerPort">Web Server Port:</label></td>
          <td class="input"><input type="number" id="webServerPort" name="webServerPort" min="1" max="65535" value=")rawliteral";
  form += String(config.webServerPort);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td class="label"><label for="firmware_name">Firmware File:</label></td>
          <td class="input"><input type="text" id="firmware_name" name="firmware_name" value=")rawliteral";
  form += String(config.firmware_name);
  form += R"rawliteral("></td>
        </tr>
        <tr>
          <td></td>
          <td>
            <table style="width:100%; border:none; padding:0; margin:0;">
              <tr>
                <td style="padding:0; border:none; text-align:left;">
                  <button type="button" style="background-color:#2196F3; color:white; border:none; border-radius:4px; padding:8px 16px; cursor:pointer; margin-right:10px;" onclick="resetDefaults()">Reset to Defaults</button>
                </td>
                <td style="padding:0; border:none; text-align:right;">
                  <button type="submit" name="restart" value="1" style="background-color:#4CAF50; color:white; border:none; border-radius:4px; padding:8px 16px; cursor:pointer;">Save and Restart</button>
                </td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </form>
    <div style="text-align:center; margin-top:20px;">
      <span style="font-size:small; color:#666;">&copy; 2025 R. Zuehlsdorff</span>
    </div>
  </div>
  <script>
    // Try to display the current web server IP in the form
    document.addEventListener("DOMContentLoaded", function() {
      var ip = window.location.hostname;
      var ipField = document.getElementById("webServerIp");
      if(ipField) ipField.textContent = ip;
    });
  </script>
</body>
</html>
)rawliteral";
  return form;
}
#endif // OTA_WEBFORM_H