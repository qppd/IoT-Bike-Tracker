// Sim800L.cpp
// Implementation for SIM800L GSM module

#include "Sim800L.h"

Sim800L::Sim800L(SoftwareSerial &serial) : gsmSerial(serial) {}

void Sim800L::begin(long baudrate) {
    gsmSerial.begin(baudrate);
}

void Sim800L::sendSMS(const String &number, const String &message) {
    gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
    delay(100);
    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(number);
    gsmSerial.println("\"");
    delay(100);
    gsmSerial.print(message);
    gsmSerial.write(26); // Ctrl+Z to send
    delay(1000);
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
