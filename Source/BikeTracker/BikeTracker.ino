
// BikeTracker.ino
#include <Arduino.h>
#include "Neo6mGPS.h"
#include "Sim800L.h"


// ESP32: Define pins for hardware serial ports
// GPS: TX=17, RX=16; GSM: TX=26, RX=27 (adjust if needed)
HardwareSerial SerialGPS(1);
HardwareSerial SerialGSM(2);
Neo6mGPS gps(SerialGPS);
Sim800L gsm(SerialGSM);

void setup() {
	Serial.begin(9600); // Debug serial
		// Begin Serial ports with pin assignments
		SerialGPS.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
		SerialGSM.begin(9600, SERIAL_8N1, 27, 26); // RX, TX
		gps.begin(9600);
		gsm.begin(9600);
	Serial.println("BikeTracker test start");
	// Optionally send a test SMS (uncomment to use)
	// gsm.sendSMS("+1234567890", "Test message from BikeTracker");
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
