// Neo6mGPS.cpp
// Implementation for Neo6m GPS module

#include "Neo6mGPS.h"

Neo6mGPS::Neo6mGPS(HardwareSerial &serial) : gpsSerial(serial) {}

void Neo6mGPS::begin(long baudrate) {
    gpsSerial.begin(baudrate);
}

bool Neo6mGPS::available() {
    return gpsSerial.available();
}

String Neo6mGPS::read() {
    if (gpsSerial.available()) {
        return gpsSerial.readStringUntil('\n');
    }
    return "";
}
