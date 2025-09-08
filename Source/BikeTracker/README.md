# Bike Tracker

# Bike Tracker

An advanced, solar-powered GPS bike tracker designed for real-world use. This project features live location tracking, GSM communication, and robust hardware for outdoor environments. Developed by [qppd](https://github.com/qppd).

## Features

- GPS tracking with Neo6m module
- IoT via SIM800L for remote updates
- Solar charging for extended battery life
- Dual 18650 lithium battery support
- 3D-printed, water-resistant (not waterproof) case and clamp for bike mounting
- ESP32-based

## Hardware Components

- 2x Mini Solar Panel
- 2x 18650 Lithium Batteries
- 1x Charging Module (for 18650 cells)
- 2x Boost Converters
- 1x ESP32 Development Board
- 1x Neo6m GPS Module
- 1x SIM800L GSM Module
- 1x 3D-Printed Water-Resistant Case & Clamp (for bike mounting)

## Getting Started

### Prerequisites

- Arduino IDE
- ESP32 board support installed
- Required libraries for GPS and GSM modules

### Cloning the Repository

```sh
git clone https://github.com/qppd/IoT-Bike-Tracker.git
cd IoT-Bike-Tracker/Source/BikeTracker
```

### Hardware Assembly


1. Solder the 18650 batteries to the charging module.
2. Connect the solar panel to the charging module input.
3. Use boost converters to supply 5V to ESP32, GPS, and GSM modules.
4. Connect Neo6m GPS and SIM800L to ESP32 as per the wiring diagram (see below).
5. Assemble all components into the 3D-printed case and clamp securely to your bike.

#### Wiring Diagram

![Wiring Diagram](https://github.com/qppd/IoT-Bike-Tracker/blob/main/Diagrams/Wiring.png)

### Software Upload

1. Open `BikeTracker.ino` in Arduino IDE.
2. Select the correct ESP32 board and port.
3. Install required libraries (see below).
4. Upload the code to your ESP32.

### Required Libraries

- TinyGPS++
- HardwareSerial
- Any other dependencies as specified in the code

## Usage

Once powered, the device will acquire GPS location and send updates via GSM every 30(increased for better battery life span) minutes. Mount the device on your bike using the provided clamp.

## 3D Printed Case

The case is designed to be water-resistant (not waterproof) and suitable for outdoor use. Ensure all seals are properly fitted.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License.

## Developers

Developed and maintained by [qppd](https://github.com/qppd).
