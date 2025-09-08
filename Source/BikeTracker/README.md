# Bike Tracker

This project is an Arduino-based bike tracking system. It uses a Neo-6M GPS module to obtain location data and a SIM800L GSM module to send the data to a remote server or phone via SMS or GPRS.

## Features
- Real-time GPS tracking
- GSM communication for remote data transmission
- Modular code structure (separate files for GPS and GSM modules)

## File Structure
- `BikeTracker.ino` - Main Arduino sketch
- `Neo6mGPS.cpp` / `Neo6mGPS.h` - GPS module interface
- `Sim800L.cpp` / `Sim800L.h` - GSM module interface

## Hardware Requirements
- Arduino board (Uno, Nano, etc.)
- Neo-6M GPS module
- SIM800L GSM module
- SIM card with data/SMS plan
- Power supply (suitable for GSM module)

## Getting Started
1. Connect the GPS and GSM modules to the Arduino as per their datasheets.
2. Update the code in `BikeTracker.ino` with your phone number or server details.
3. Upload the code to your Arduino board.
4. Power the system and monitor location updates via SMS or server.

## License
This project is open source and available under the MIT License.
