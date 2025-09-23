
// BikeTracker.ino
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "PinConfig.h"
#include "Neo6mGPS.h"
#include "Sim800L.h"


// NodeMCU v3 1.0: Pins defined in PinConfig.h
SoftwareSerial SerialGPS(GPS_RX_PIN, GPS_TX_PIN); // RX, TX
SoftwareSerial SerialGSM(GSM_RX_PIN, GSM_TX_PIN); // RX, TX
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
