// Sim800L.h
// Header for SIM800L GSM module

#ifndef SIM800L_H
#define SIM800L_H

#include <Arduino.h>
#include <SoftwareSerial.h>

enum GSMStatus {
    GSM_INIT,
    GSM_READY,
    GSM_ERROR,
    GSM_NO_NETWORK,
    GSM_NETWORK_CONNECTED
};

class Sim800L {
public:
    Sim800L(SoftwareSerial &serial);
    void begin(long baudrate = 9600);
    bool initialize();
    GSMStatus getStatus();
    void sendSMS(const String &number, const String &message);
    bool sendLocationSMS(const String &number, const String &location, const String &alertType = "");
    bool available();
    String read();
    bool isNetworkConnected();
    int getSignalStrength();
    void powerOn();
    void powerOff();
    String getIMEI();
    bool sendATCommand(const String &command, const String &expectedResponse = "OK", int timeout = 5000);
    
private:
    SoftwareSerial &gsmSerial;
    GSMStatus status;
    String lastResponse;
    unsigned long lastCommandTime;
    bool waitForResponse(const String &expected, int timeout);
    void clearBuffer();
};

#endif // SIM800L_H
