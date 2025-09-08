// Sim800L.h
// Header for SIM800L GSM module

#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>

class Sim800L {
public:
    Sim800L(HardwareSerial &serial);
    void begin(long baudrate = 9600);
    void sendSMS(const String &number, const String &message);
    bool available();
    String read();
private:
    HardwareSerial &gsmSerial;
};

#endif // SIM800L_H
