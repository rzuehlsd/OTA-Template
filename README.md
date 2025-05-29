# OTA Update System for ESP8266, ESP32-C3, ESP32-S3

This project provides a complete solution for **Over-the-Air (OTA) firmware updates** for ESP8266/ESP32C3/ESP32S3 devices, including a web-based configuration interface and a Node.js OTA server.
The Web Server provides a user hook for easy custom extensions to the server, e.g. for additional configurations.

REMARK:  The framework should also fit other ESP32 variants, but has only been tested for the chip versions above.

---

## Contents

- [Overview](#overview)
- [File Structure](#file-structure)
- [ESP8266 Firmware Setup](#esp8266-firmware-setup)
- [Web Configuration Interface](#web-configuration-interface)
- [OTA Server Setup (Node.js)](#ota-server-setup-nodejs)
- [How OTA Works](#how-ota-works)
- [Custom Extensions](#custom-extensions)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## Overview

- **OTA framework**: Connects to WiFi, checks for new firmware on the OTA server, and updates itself if a newer version is available. All settings can be configured via a web interface.
- **OTA Server (Node.js)**: Hosts the firmware binary and version file, serving them to ESP8266 devices for updates.

- A more detailed description could be found in the different sources and configuration files. The file OTA_TEST.cpp serves as an example to demonstrate the usage of the OTA Template, the node.js OTA Server and the extension of the integrated WebServer to host and process user specific pages. Therefore if you want to cretae your own OTA enabled application it is recommended to start with the OTA_TEST file and add your specific code.

- Please note that the current version of the OTAServer just provides ota updates from the subdirectory "updates". To accomodate update packages for multiple ota enabled applications just provide more specific names than "firmware.bin" and "firmware.bin.version" in the base configuration file "config.h" of the OTA enabled application. Than place the bin and version files in the updates directory of the node.js OTA Server. It is also suggested to name the firmware bin file according to application and chip variant. An update from a firmware version intended for a differen chip variant will certainly fail! That's all!
 
---

## File Structure

### ESP8266 Project

```
OTA-Template/
├── src/
│   ├── config.h              # Configuration macros and defaults
│   ├── OTA_WebConfig.h/cpp   # Configuration logic and web server
│   ├── OTA_WebForm.h         # HTML for the configuration web page
│   ├── OTA_Template.h/cpp    # OTA update logic
│   ├── OTA_TEST.cpp          # Main application entry point
│   └── ...                   # Other source files
└── README                    # This file
```

### OTA Server Project

```
OTAServer/
├── ota-server.js             # Node.js OTA server
└── updates/
    ├── firmware.bin          # Firmware binary to be served
    └── firmware.bin.version  # Text file with the firmware version (e.g., 1.1.0)
```

---

## ESP8266 Firmware Setup

1. **Clone or download this repository.**
2. Open the project in Arduino IDE or PlatformIO.
3. Edit `src/config.h` to set your default WiFi and OTA server settings.
4. Connect your ESP8266 board to your computer.
5. Select the correct board and port in your IDE.
6. Upload the firmware to your ESP8266.

---

## Web Configuration Interface

1. **Connect the ESP8266 to your WiFi network.**
   - By default, it uses the SSID and password defined in `config.h`.
2. **Find the device's IP address.**
   - Check your router or use a serial monitor.
3. **Open the configuration page in your browser:**
   - Go to `http://<device-ip>/ota`
4. **Configure settings as needed and save.**
   - The device will restart if required.

---

## OTA Server Setup (Node.js)

1. **Install Node.js**  
   Download and install from [nodejs.org](https://nodejs.org/).

2. **Set up the OTA server directory:**
   ```sh
   mkdir OTAServer
   cd OTAServer
   mkdir updates
   ```

3. **Place your firmware and version file in `updates/`:**
   - `firmware.bin` — The compiled firmware binary.
   - `firmware.bin.version` — A text file containing the version string (e.g., `1.1.0`).

4. **Create `ota-server.js` with the following content:**
   (See `ota-server.js` in this repository for the full code.)

5. **Install dependencies:**
   ```sh
   npm init -y
   npm install express
   ```

6. **Start the OTA server:**
   ```sh
   node ota-server.js
   ```
   The server will listen on port 3000 by default and serve files from the `updates` directory.

---

## How OTA Works

- The ESP8266 device periodically checks the OTA server for a new firmware version.
- If a newer version is found, it downloads `firmware.bin` and updates itself.
- All configuration (WiFi, OTA server, update interval, etc.) can be managed via the web interface at `/ota`.

---

## Custom Extensions

You can add your own web endpoints and logic by using the `registerCustomEndpoint()` function in your `userSetup()` (see `OTA_TEST.cpp`):

```cpp
registerCustomEndpoint("/hello", []() {
  server.send(200, "text/plain", "Hello, world!");
}, HTTP_GET);
```

---

## Troubleshooting

- **Device not connecting to WiFi?**  
  Double-check your SSID and password in `config.h` or via the web interface.

- **OTA not working?**  
  - Ensure the OTA server is running and accessible from the ESP8266.
  - Check that `firmware.bin` and `firmware.bin.version` exist in the `updates` directory.
  - Make sure the OTA server port matches `OTA_PORT` in `config.h`.

- **Web interface not loading?**  
  - Confirm the device's IP address.
  - Ensure your computer is on the same network as the ESP8266.

---

## License

MIT License

---

## Author

R. Zuehlsdorff, 2025

