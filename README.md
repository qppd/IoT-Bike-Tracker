# 🚴‍♂️ IoT Bike Tracker

<div align="center">

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![ESP8266](https://img.shields.io/badge/ESP8266-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0.0-blue.svg?style=for-the-badge)

**An advanced, solar-powered GPS bike tracker with real-time location monitoring and GSM communication capabilities**

[🚀 Quick Start](#-quick-start) • [📋 Features](#-features) • [🔧 Hardware](#-hardware-requirements) • [📖 Documentation](#-api-documentation) • [🤝 Contributing](#-contributing)

</div>

---

## 📋 Table of Contents

- [🌟 Overview](#-overview)
- [✨ Features](#-features)
- [🔧 Hardware Requirements](#-hardware-requirements)
- [💻 Software Requirements](#-software-requirements)
- [🚀 Quick Start](#-quick-start)
- [⚡ Hardware Assembly](#-hardware-assembly)
- [📡 Pin Configuration](#-pin-configuration)
- [🛠️ Software Installation](#️-software-installation)
- [📖 API Documentation](#-api-documentation)
- [🎯 Usage Examples](#-usage-examples)
- [🔍 Troubleshooting](#-troubleshooting)
- [📋 Changelog](#-changelog)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)
- [🙏 Acknowledgments](#-acknowledgments)

---

## 🌟 Overview

The **IoT Bike Tracker** is a comprehensive solution for bicycle security and monitoring, featuring real-time GPS tracking, GSM communication, and sustainable solar power. Built with the ESP8266-based NodeMCU v3.1.0, this project combines robust hardware design with efficient software architecture for reliable outdoor operation.

### 🎯 Key Highlights

- 🛰️ **Real-time GPS Tracking** with Neo6m module
- 📱 **GSM Communication** via SIM800L for remote monitoring
- ☀️ **Solar-powered** with dual 18650 battery backup
- 🔋 **Extended Battery Life** with optimized power management
- 🏠 **Weather-resistant** 3D-printed enclosure
- 🔧 **Modular Code Architecture** for easy customization
- 🚲 **Secure Bike Mounting** with custom clamp design

---

## ✨ Features

### 🛰️ **GPS Tracking System**
- High-precision location tracking with Neo6m GPS module
- Real-time coordinate acquisition and processing
- Configurable update intervals for battery optimization
- NMEA data parsing and validation

### 📡 **GSM Communication**
- SMS-based location reporting via SIM800L module
- **HTTP POST requests** to web APIs over GPRS
- Real-time data transmission to cloud services
- Remote command processing capability
- Network status monitoring and error handling
- Configurable message intervals (default: 30 minutes)

### ⚡ **Power Management**
- Dual solar panel charging system
- 2S BMS protection for 18650 lithium batteries
- Buck converter voltage regulation
- Low-power sleep modes for extended operation

### 🏗️ **Hardware Design**
- Robust PCB layout optimized for outdoor use
- Water-resistant (IP54) 3D-printed enclosure
- Secure bike mounting system with anti-theft features
- Temperature-resistant component selection

### 💾 **Software Architecture**
- Object-oriented C++ design with separate classes
- Modular GPS and GSM communication libraries
- Comprehensive error handling and recovery
- Serial debugging and monitoring capabilities

---

## 🔧 Hardware Requirements

### 📦 **Bill of Materials (BOM)**

| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| 🌞 **Mini Solar Panel** | 2x | 5V 1W Polycrystalline | Primary power source |
| 🔋 **18650 Lithium Battery** | 2x | 3.7V 3000mAh Protected | Energy storage |
| ⚡ **2S BMS Module** | 1x | 7.4V 8A Protection | Battery management |
| 🔌 **Buck Converter** | 2x | DC-DC Step Down 3.3V/5V | Voltage regulation |
| 🧠 **NodeMCU v3.1.0** | 1x | ESP8266 Development Board | Main microcontroller |
| 🛰️ **Neo6m GPS Module** | 1x | UART GPS Receiver | Location tracking |
| 📱 **SIM800L GSM Module** | 1x | Quad-band GSM/GPRS | Communication |
| 🏠 **3D Printed Case** | 1x | Water-resistant ABS/PETG | Protection enclosure |
| 🔧 **Bike Mounting Clamp** | 1x | Adjustable Security Clamp | Secure attachment |

### 🔌 **Power Specifications**
- **Input Voltage**: 5V (Solar) / 7.4V (Battery)
- **Operating Voltage**: 3.3V (MCU) / 5V (Modules)
- **Power Consumption**: ~200mA (Active) / ~50μA (Sleep)
- **Battery Life**: 7-14 days (weather dependent)

### ⚡ **Feature Compatibility**

#### ✅ **Basic Hardware (GPS + GSM + MCU)**
- 🛰️ Real-time GPS tracking
- 📱 SMS notifications and HTTP API
- 🚶 Motion detection (GPS-based)
- 🏎️ Speed monitoring and alerts
- 🗺️ Geofencing with breach detection
- 📊 System status monitoring

#### ⭐ **Advanced Hardware (Additional Sensors Required)**
- 🔋 **Battery Monitoring** - Requires voltage/current sensor
- 🚨 **Theft Detection** - Requires accelerometer/gyroscope
- 🔊 **Audio Alerts** - Requires buzzer/speaker
- 💡 **Status LEDs** - Requires LED indicators

**Note**: The current implementation focuses on GPS+GSM functionality. Additional sensors can be added for enhanced features.

---

## 💻 Software Requirements

### 🛠️ **Development Environment**
- **Arduino IDE** 1.8.19 or later
- **ESP8266 Board Package** 3.0.0 or later
- **USB to Serial Driver** (CP2102/CH340)

### 📚 **Required Libraries**

```cpp
// Core Libraries (Built-in)
#include <Arduino.h>
#include <SoftwareSerial.h>

// Project-specific Libraries (Included)
#include "Neo6mGPS.h"     // GPS module handler
#include "Sim800L.h"      // GSM module handler
```

### 📋 **Library Installation**
```bash
# Install ESP8266 Board Package
# File → Preferences → Additional Board Manager URLs:
# http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

---

## 🚀 Quick Start

### 1️⃣ **Clone Repository**
```bash
git clone https://github.com/qppd/IoT-Bike-Tracker.git
cd IoT-Bike-Tracker/Source/BikeTracker
```

### 2️⃣ **Hardware Setup**
- Assemble components according to wiring diagram
- Connect solar panels and battery system
- Mount GPS and GSM antennas appropriately

### 3️⃣ **Software Upload**
- Open `BikeTracker.ino` in Arduino IDE
- Select **NodeMCU 1.0 (ESP-12E Module)** board
- Choose appropriate COM port
- Upload code to device

### 4️⃣ **Initial Configuration**
- Insert activated SIM card into SIM800L module
- Power on the system and monitor serial output
- Test GPS acquisition and GSM connectivity

---

## ⚡ Hardware Assembly

### 🔌 **Power System Connection**

```
Solar Panels → Buck Converter → 2S BMS → 18650 Batteries
     ↓              ↓             ↓           ↓
   5V Input    →  Regulated   →  7.4V    →  Power
                   Voltage      Storage     Backup
```

### 📡 **Module Integration**

1. **GPS Module (Neo6m)**
   - Connect VCC to 3.3V regulated output
   - Connect GND to common ground
   - Wire RX/TX to designated GPIO pins

2. **GSM Module (SIM800L)**
   - Connect VCC to 5V regulated output (high current)
   - Ensure stable power supply with capacitors
   - Connect antenna for optimal signal reception

3. **NodeMCU Integration**
   - Power via VIN pin (5V) or 3.3V pin
   - Configure GPIO pins for SoftwareSerial communication
   - Implement proper grounding across all modules

---

## 📡 Pin Configuration

### 🎛️ **NodeMCU v3.1.0 Pin Mapping**

| Module | Function | NodeMCU Pin | GPIO | Notes |
|--------|----------|-------------|------|-------|
| **GPS** | RX | D2 | GPIO4 | SoftwareSerial RX |
| **GPS** | TX | D3 | GPIO0 | SoftwareSerial TX |
| **GSM** | RX | D5 | GPIO14 | SoftwareSerial RX |
| **GSM** | TX | D6 | GPIO12 | SoftwareSerial TX |
| **Power** | VIN | VIN | - | 5V Input |
| **Debug** | USB | USB | - | Serial Monitor |

### ⚠️ **Important Notes**
- Avoid using GPIO0, GPIO2, GPIO15 during boot
- Ensure proper pull-up/pull-down resistors where needed
- Use level shifters if voltage mismatch occurs

---

## 🛠️ Software Installation

### 📥 **Step-by-Step Installation**

1. **Prepare Development Environment**
   ```bash
   # Install Arduino IDE
   # Add ESP8266 board support
   # Install required drivers
   ```

2. **Configure Board Settings**
   ```
   Board: "NodeMCU 1.0 (ESP-12E Module)"
   CPU Frequency: "80 MHz"
   Flash Size: "4MB (FS:2MB OTA:~1019KB)"
   Upload Speed: "115200"
   ```

3. **Compile and Upload**
   ```cpp
   // Verify code compilation
   // Select correct COM port
   // Upload to device
   // Monitor serial output
   ```

### 🔧 **Configuration Options**

```cpp
// In BikeTracker.ino, modify these parameters:
#define GPS_BAUD_RATE 9600
#define GSM_BAUD_RATE 9600
#define DEBUG_BAUD_RATE 9600
#define UPDATE_INTERVAL 1800000  // 30 minutes in milliseconds
```

---

## 📖 API Documentation

### 📍 **Neo6mGPS Class**

#### **Constructor**
```cpp
Neo6mGPS(SoftwareSerial &serial)
```
Initializes GPS module with specified serial interface.

#### **Methods**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `begin(long baudrate)` | `void` | Initialize GPS serial communication |
| `available()` | `bool` | Check if GPS data is available |
| `read()` | `String` | Read GPS NMEA sentence |

#### **Usage Example**
```cpp
SoftwareSerial SerialGPS(D2, D3);
Neo6mGPS gps(SerialGPS);

void setup() {
    gps.begin(9600);
}

void loop() {
    if (gps.available()) {
        String gpsData = gps.read();
        // Process GPS data
    }
}
```

### 📱 **Sim800L Class**

#### **Constructor**
```cpp
Sim800L(SoftwareSerial &serial)
```
Initializes GSM module with specified serial interface.

#### **Methods**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `begin(long baudrate)` | `void` | Initialize GSM serial communication |
| `sendSMS(String number, String message)` | `void` | Send SMS to specified number |
| `available()` | `bool` | Check if GSM data is available |
| `read()` | `String` | Read GSM response data |
| `initializeGPRS(String apn, String user, String pass)` | `bool` | Initialize GPRS connection |
| `sendHTTPPOST(String url, String jsonData, String &response)` | `bool` | Send HTTP POST request |
| `sendLocationHTTP(String url, String deviceId, float lat, float lon, String alertType)` | `bool` | Send location data to web API |
| `disconnectGPRS()` | `void` | Disconnect GPRS connection |

#### **Usage Example**
```cpp
SoftwareSerial SerialGSM(D5, D6);
Sim800L gsm(SerialGSM);

void setup() {
    gsm.begin(9600);
}

void sendLocation() {
    gsm.sendSMS("+1234567890", "Bike Location: lat,lng");
}
```

### 🌐 **HTTP API Integration**

The bike tracker now supports sending location data to web APIs via HTTP POST requests over GPRS.

#### **Configuration**

Edit `APIConfig.h` to configure your web API:

```cpp
// Your web API endpoint
#define WEB_API_URL "https://your-api.com/api/tracker"

// Unique device identifier
#define DEVICE_ID "BIKE_TRACKER_001"

// Mobile carrier APN
#define APN_NAME "internet"

// Update interval (milliseconds)
#define HTTP_UPDATE_INTERVAL 30000  // 30 seconds

// Enable/disable HTTP functionality
#define HTTP_ENABLED true
```

#### **JSON Data Format**

The tracker sends location data in the following JSON format:

```json
{
    "deviceId": "BIKE_TRACKER_001",
    "latitude": 40.7128,
    "longitude": -74.0060,
    "timestamp": "1640995200000",
    "alertType": "MOTION_DETECTED",
    "signalStrength": 25
}
```

#### **API Endpoint Requirements**

Your web API should accept POST requests with:
- **Content-Type**: `application/json`
- **HTTP Method**: `POST`
- **Expected Response**: HTTP 200-299 for success

#### **Usage Example**

```cpp
// Configure the web API in setup()
void setup() {
    tracker.setWebAPI(WEB_API_URL, DEVICE_ID, APN_NAME);
}

// Location data is automatically sent every HTTP_UPDATE_INTERVAL
// Manual sending can be triggered with:
tracker.sendLocationToAPI();
```

#### **Testing Commands**

In testing mode, use these serial commands:
- `API` - Manually send location to web API
- `STATUS` - View HTTP connection status
- `DIAG` - Check GPRS connectivity

---

## 🎯 Usage Examples

### 📍 **Basic GPS Tracking**
```cpp
void trackLocation() {
    if (gps.available()) {
        String nmea = gps.read();
        if (nmea.startsWith("$GPGGA")) {
            // Parse coordinates
            // Extract latitude/longitude
            // Store or transmit data
        }
    }
}
```

### 📱 **SMS Notifications**
```cpp
void sendAlert(String alertType) {
    String message = "BIKE ALERT: " + alertType + 
                    " Location: " + getCurrentLocation();
    gsm.sendSMS(OWNER_PHONE, message);
}
```

### ⚡ **Power Management**
```cpp
void enterSleepMode() {
    // Save current state
    // Configure wake-up sources
    ESP.deepSleep(30 * 60 * 1000000); // 30 minutes
}
```

---

## 🔍 Troubleshooting

### ❓ **Common Issues**

| Problem | Symptoms | Solution |
|---------|----------|----------|
| **GPS No Fix** | No coordinate data | • Check antenna placement<br>• Verify power supply<br>• Wait for satellite acquisition |
| **GSM No Network** | SMS not sending | • Check SIM card activation<br>• Verify network coverage<br>• Inspect antenna connection |
| **Power Issues** | Frequent resets | • Check battery voltage<br>• Verify solar panel output<br>• Inspect wiring connections |
| **Upload Failed** | Code won't upload | • Check COM port selection<br>• Try different USB cable<br>• Reset NodeMCU manually |

### 🐛 **Debug Commands**

```cpp
// Enable verbose debugging
#define DEBUG_MODE 1

// Serial monitor output
Serial.println("GPS Status: " + gps.getStatus());
Serial.println("GSM Signal: " + gsm.getSignalStrength());
Serial.println("Battery: " + getBatteryVoltage() + "V");
```

### 📞 **Support Resources**
- 📧 **Email**: Create an issue on GitHub
- 📖 **Documentation**: Check Wiki for detailed guides
- 💬 **Community**: Join discussions in Issues section

---

## 📋 Changelog

### 🔖 **Version 1.0.0** (Current)
- ✅ Initial release with basic GPS tracking
- ✅ SMS notification system implementation
- ✅ Solar power management integration
- ✅ 3D-printed case design
- ✅ Modular software architecture

### 🔮 **Planned Features** (v1.1.0)
- 🚧 Web dashboard for real-time monitoring
- 🚧 Geofencing with automatic alerts
- 🚧 Battery optimization algorithms
- 🚧 OTA (Over-The-Air) update capability
- 🚧 Enhanced security features

---

## 🤝 Contributing

We welcome contributions from the community! Here's how you can help:

### 🎯 **How to Contribute**

1. **🍴 Fork the Repository**
   ```bash
   git fork https://github.com/qppd/IoT-Bike-Tracker.git
   ```

2. **🌿 Create Feature Branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```

3. **💾 Commit Changes**
   ```bash
   git commit -m "Add amazing feature"
   ```

4. **📤 Push to Branch**
   ```bash
   git push origin feature/amazing-feature
   ```

5. **🔄 Open Pull Request**
   - Describe your changes clearly
   - Include testing information
   - Reference any related issues

### 📝 **Contribution Guidelines**

- **Code Style**: Follow Arduino/C++ conventions
- **Documentation**: Update README and comments
- **Testing**: Verify functionality before submission
- **Issues**: Use templates for bug reports and features

### 🏆 **Contributors**

Thanks to all contributors who have helped improve this project!

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 qppd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## 🙏 Acknowledgments

### 🎖️ **Special Thanks**
- **ESP8266 Community** for excellent documentation and support
- **Arduino Foundation** for the development platform
- **Open Source Hardware** movement for inspiration
- **Contributors** who have helped improve this project

### 📚 **Resources & References**
- [ESP8266 Arduino Core Documentation](https://arduino-esp8266.readthedocs.io/)
- [Neo6m GPS Module Datasheet](https://www.u-blox.com/en/docs)
- [SIM800L GSM Module Documentation](https://simcom.ee/documents/)
- [3D Printing Guidelines](https://www.thingiverse.com/)

### 🛠️ **Development Tools**
- **Arduino IDE** - Primary development environment
- **Fritzing** - Circuit design and documentation
- **Fusion 360** - 3D modeling and case design
- **VS Code** - Documentation and advanced editing

---

<div align="center">

**🚴‍♂️ Happy Tracking! 🚴‍♀️**

*Built with ❤️ by [qppd](https://github.com/qppd)*

![GitHub](https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white)
![Made with](https://img.shields.io/badge/Made%20with-Arduino-1f425f.svg?style=for-the-badge)

</div>
