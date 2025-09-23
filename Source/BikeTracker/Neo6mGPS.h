// Neo6mGPS.h
// Header for Neo6m GPS module

#ifndef NEO6MGPS_H
#define NEO6MGPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>

struct GPSData {
    bool isValid;
    float latitude;
    float longitude;
    float altitude;
    float speed;
    int satellites;
    String timestamp;
    float hdop;
};

class Neo6mGPS {
public:
    Neo6mGPS(SoftwareSerial &serial);
    void begin(long baudrate = 9600);
    bool available();
    String read();
    GPSData parseGPSData();
    bool isLocationValid();
    String getLocationString();
    float getSpeed();
    void enableGGA();
    void enableRMC();
    
private:
    SoftwareSerial &gpsSerial;
    GPSData currentData;
    bool parseGGA(String sentence);
    bool parseRMC(String sentence);
    float convertDMSToDecimal(String dms, String direction);
    String rawData;
};

#endif // NEO6MGPS_H
