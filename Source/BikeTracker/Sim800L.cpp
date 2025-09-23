// Sim800L.cpp
// Implementation for SIM800L GSM module

#include "Sim800L.h"

Sim800L::Sim800L(SoftwareSerial &serial) : gsmSerial(serial) {
    status = GSM_INIT;
    lastCommandTime = 0;
}

void Sim800L::begin(long baudrate) {
    gsmSerial.begin(baudrate);
    delay(2000);
}

bool Sim800L::initialize() {
    clearBuffer();
    
    // Test AT communication
    if (!sendATCommand("AT", "OK", 3000)) {
        status = GSM_ERROR;
        return false;
    }
    
    // Disable echo
    sendATCommand("ATE0", "OK", 3000);
    
    // Set SMS text mode
    if (!sendATCommand("AT+CMGF=1", "OK", 3000)) {
        status = GSM_ERROR;
        return false;
    }
    
    // Check network registration
    if (sendATCommand("AT+CREG?", "+CREG: 0,1", 10000) || 
        sendATCommand("AT+CREG?", "+CREG: 0,5", 10000)) {
        status = GSM_NETWORK_CONNECTED;
        return true;
    } else {
        status = GSM_NO_NETWORK;
        return false;
    }
}

GSMStatus Sim800L::getStatus() {
    return status;
}

bool Sim800L::sendATCommand(const String &command, const String &expectedResponse, int timeout) {
    clearBuffer();
    gsmSerial.println(command);
    lastCommandTime = millis();
    
    return waitForResponse(expectedResponse, timeout);
}

bool Sim800L::waitForResponse(const String &expected, int timeout) {
    String response = "";
    unsigned long startTime = millis();
    
    while (millis() - startTime < timeout) {
        while (gsmSerial.available()) {
            char c = gsmSerial.read();
            response += c;
            
            if (response.indexOf(expected) >= 0) {
                lastResponse = response;
                return true;
            }
            
            if (response.indexOf("ERROR") >= 0) {
                lastResponse = response;
                return false;
            }
        }
        delay(10);
    }
    
    lastResponse = response;
    return false;
}

void Sim800L::clearBuffer() {
    while (gsmSerial.available()) {
        gsmSerial.read();
    }
}

void Sim800L::sendSMS(const String &number, const String &message) {
    if (status != GSM_NETWORK_CONNECTED) {
        return;
    }
    
    clearBuffer();
    
    // Set SMS text mode
    gsmSerial.println("AT+CMGF=1");
    delay(1000);
    
    // Set recipient
    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(number);
    gsmSerial.println("\"");
    delay(1000);
    
    // Send message
    gsmSerial.print(message);
    gsmSerial.write(26); // Ctrl+Z to send
    delay(5000);
    
    // Clear any response
    clearBuffer();
}

bool Sim800L::sendLocationSMS(const String &number, const String &location, const String &alertType) {
    if (status != GSM_NETWORK_CONNECTED) {
        return false;
    }
    
    String message = "BikeTracker Alert";
    if (alertType.length() > 0) {
        message += " - " + alertType;
    }
    message += "\nLocation: " + location;
    message += "\nTime: " + String(millis() / 1000) + "s";
    
    sendSMS(number, message);
    return true;
}

bool Sim800L::available() {
    return gsmSerial.available();
}

String Sim800L::read() {
    if (gsmSerial.available()) {
        return gsmSerial.readStringUntil('\n');
    }
    return "";
}

bool Sim800L::isNetworkConnected() {
    return (status == GSM_NETWORK_CONNECTED);
}

int Sim800L::getSignalStrength() {
    if (sendATCommand("AT+CSQ", "+CSQ:", 3000)) {
        // Parse signal strength from response
        int start = lastResponse.indexOf("+CSQ: ") + 6;
        int end = lastResponse.indexOf(",", start);
        if (start > 5 && end > start) {
            return lastResponse.substring(start, end).toInt();
        }
    }
    return -1;
}

String Sim800L::getIMEI() {
    if (sendATCommand("AT+GSN", "OK", 3000)) {
        // Extract IMEI from response
        int start = lastResponse.indexOf('\n') + 1;
        int end = lastResponse.indexOf('\n', start);
        if (start > 0 && end > start) {
            return lastResponse.substring(start, end);
        }
    }
    return "Unknown";
}

void Sim800L::powerOn() {
    // Implementation depends on hardware setup
    // Usually involves toggling a power pin
}

void Sim800L::powerOff() {
    sendATCommand("AT+CPOWD=1", "OK", 5000);
}
