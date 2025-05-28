/**
 * ota-server.js
 *
 * This file implements a simple OTA (Over-the-Air) Update Server for ESP8266/ESP32 devices using Node.js and Express.
 *
 * Purpose:
 * - Serves firmware binary files and version information to ESP8266/ESP32 devices for remote firmware updates.
 * - Allows devices to check for new firmware versions and download updates automatically.
 *
 * Main Features:
 * - Endpoint to download firmware binaries:      GET /firmware/:filename
 * - Endpoint to get firmware version string:     GET /version/:filename
 * - Static access to the updates directory:      GET /updates/...
 * - Logs all incoming HTTP requests and file accesses.
 *
 * Usage:
 * 1. Place your firmware binary (e.g., firmware.bin) and its version file (e.g., firmware.bin.version) in the 'updates' directory.
 * 2. Start the server with: node ota-server.js
 * 3. Configure your ESP8266/ESP32 devices to use this server for OTA updates.
 *
 * Author: R. Zuehlsdorff
 */

const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

// --- Configuration ---
const UPDATES_DIR = path.join(__dirname, 'updates'); // Directory for firmware updates
const FIRMWARE_FILE = path.join(UPDATES_DIR, 'firmware.bin');
const VERSION_FILE = path.join(UPDATES_DIR, 'firmware.bin.version');

// --- Middleware ---

// Logs all incoming requests
app.use((req, res, next) => {
  console.log(`Incoming request: ${req.method} ${req.url}`);
  next();
});

// Allows static access to the updates folder (e.g., for manual testing)
app.use('/updates', express.static(UPDATES_DIR));

// --- Helper Functions ---

/**
 * Reads the firmware version from a file.
 * @param {string} file - Path to the version file
 * @returns {string} - Version string or 'unknown'
 */
function getFirmwareVersion(file) {
  if (fs.existsSync(file)) {
    try {
      return fs.readFileSync(file, 'utf8').trim();
    } catch (err) {
      console.error('Error reading version file:', err);
      return 'unknown';
    }
  } else {
    console.warn('Version file not found.');
    return 'unknown';
  }
}

// --- API Endpoints ---

/**
 * Serves the requested firmware file as a download.
 * Example: GET /firmware/firmware.bin
 */
app.get('/firmware/:filename', (req, res) => {
  const file = path.join(UPDATES_DIR, req.params.filename);
  console.log('Requested firmware file:', file);
  if (fs.existsSync(file)) {
    console.log(`Serving firmware file: ${file}`);
    res.setHeader('Content-Type', 'application/octet-stream');
    res.download(file, (err) => {
      if (err) {
        console.error('Error sending firmware file:', err);
      } else {
        console.log('Firmware file sent successfully.');
      }
    });
  } else {
    res.status(404).send('Firmware file not found.');
  }
});

/**
 * Returns the version number of a firmware file.
 * Example: GET /version/firmware.bin.version
 */
app.get('/version/:filename', (req, res) => {
  const file = path.join(UPDATES_DIR, req.params.filename);
  console.log('Requested version file:', file);
  const firmwareVersion = getFirmwareVersion(file);
  res.send(firmwareVersion);
});

// --- Server Startup ---

app.listen(PORT, '0.0.0.0', () => {
  console.log(`OTA Update Server running at http://localhost:${PORT}`);
  console.log(`Firmware directory: ${UPDATES_DIR}`);
  console.log(`Firmware file: ${FIRMWARE_FILE}`);
  console.log(`Firmware version file: ${VERSION_FILE}`);
});
