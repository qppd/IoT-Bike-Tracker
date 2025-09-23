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
    
    // Enhanced HTTP/GPRS functions
    bool initializeGPRS(const String &apn, const String &username = "", const String &password = "");
    bool isGPRSConnected();
    bool reconnectGPRS();
    bool checkInternetConnectivity();
    bool sendHTTPPOST(const String &url, const String &jsonData, String &response);
    bool sendHTTPGET(const String &url, String &response);
    bool sendLocationHTTP(const String &url, const String &deviceId, float latitude, float longitude, const String &alertType = "");
    void disconnectGPRS();
    String getLocalIP();
    void enableAutoTimeSync();
    bool setHTTPHeaders(const String &headers);
    
    // Connection state management
    bool maintainConnection();
    void resetConnection();
    unsigned long getLastDataActivity();
    
private:
    SoftwareSerial &gsmSerial;
    GSMStatus status;
    String lastResponse;
    unsigned long lastCommandTime;
    unsigned long lastDataActivity;
    String currentAPN;
    String currentUsername;
    String currentPassword;
    bool gprsConnected;
    bool waitForResponse(const String &expected, int timeout);
    void clearBuffer();
    bool ensureGPRSConnection();
    int extractHTTPStatusCode(const String &response);
    bool performHTTPRequest(const String &method, const String &url, const String &data, String &response);
};

#endif // SIM800L_H
