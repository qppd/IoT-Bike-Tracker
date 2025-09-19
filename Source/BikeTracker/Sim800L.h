// Sim800L.h
// Header for SIM800L GSM module

#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class Sim800L {
public:
    Sim800L(SoftwareSerial &serial);
    void begin(long baudrate = 9600);
    void sendSMS(const String &number, const String &message);
    bool available();
    String read();
private:
    SoftwareSerial &gsmSerial;
};

#endif // SIM800L_H
