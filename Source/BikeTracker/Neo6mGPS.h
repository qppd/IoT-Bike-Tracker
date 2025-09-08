// Neo6mGPS.h
// Header for Neo6m GPS module

#ifndef NEO6MGPS_H
#define NEO6MGPS_H

#include <Arduino.h>

class Neo6mGPS {
public:
    Neo6mGPS(HardwareSerial &serial);
    void begin(long baudrate = 9600);
    bool available();
    String read();
private:
    HardwareSerial &gpsSerial;
};

#endif // NEO6MGPS_H
