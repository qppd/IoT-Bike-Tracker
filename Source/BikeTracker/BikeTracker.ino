
// BikeTracker.ino
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Neo6mGPS.h"
#include "Sim800L.h"


// NodeMCU v3 1.0: Define pins for software serial ports
// GPS: RX=D2, TX=D3; GSM: RX=D5, TX=D6
SoftwareSerial SerialGPS(D2, D3); // RX, TX
SoftwareSerial SerialGSM(D5, D6); // RX, TX
Neo6mGPS gps(SerialGPS);
Sim800L gsm(SerialGSM);

void setup() {
	Serial.begin(9600); // Debug serial
		// Begin Serial ports with pin assignments
		SerialGPS.begin(9600);
		SerialGSM.begin(9600);
		gps.begin(9600);
		gsm.begin(9600);
	Serial.println("BikeTracker test start");
	
	// gsm.sendSMS("+1234567890", "Test message");
}

void loop() {
	// Test GPS
	if (gps.available()) {
		String gpsData = gps.read();
		Serial.print("GPS: ");
		Serial.println(gpsData);
	}
	// Test GSM
	if (gsm.available()) {
		String gsmData = gsm.read();
		Serial.print("GSM: ");
		Serial.println(gsmData);
	}
	delay(1000);
}
