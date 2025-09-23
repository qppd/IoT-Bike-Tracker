// Neo6mGPS.cpp
// Implementation for Neo6m GPS module

#include "Neo6mGPS.h"

Neo6mGPS::Neo6mGPS(SoftwareSerial &serial) : gpsSerial(serial) {
    currentData.isValid = false;
    currentData.latitude = 0.0;
    currentData.longitude = 0.0;
    currentData.altitude = 0.0;
    currentData.speed = 0.0;
    currentData.satellites = 0;
    currentData.hdop = 0.0;
    rawData = "";
}

void Neo6mGPS::begin(long baudrate) {
    gpsSerial.begin(baudrate);
    delay(1000);
    enableGGA();
    enableRMC();
}

void Neo6mGPS::enableGGA() {
    gpsSerial.println("$PUBX,40,GGA,0,1,0,0*5A"); // Enable GGA messages
    delay(100);
}

void Neo6mGPS::enableRMC() {
    gpsSerial.println("$PUBX,40,RMC,0,1,0,0*47"); // Enable RMC messages
    delay(100);
}

bool Neo6mGPS::available() {
    return gpsSerial.available();
}

String Neo6mGPS::read() {
    if (gpsSerial.available()) {
        char c = gpsSerial.read();
        if (c == '$') {
            rawData = "";
        }
        rawData += c;
        if (c == '\n') {
            String completeSentence = rawData;
            rawData = "";
            return completeSentence;
        }
    }
    return "";
}

GPSData Neo6mGPS::parseGPSData() {
    while (gpsSerial.available()) {
        String sentence = read();
        if (sentence.length() > 0) {
            if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
                parseGGA(sentence);
            } else if (sentence.startsWith("$GPRMC") || sentence.startsWith("$GNRMC")) {
                parseRMC(sentence);
            }
        }
    }
    return currentData;
}

bool Neo6mGPS::parseGGA(String sentence) {
    // Parse GGA sentence: $GPGGA,time,lat,lat_dir,lon,lon_dir,quality,satellites,hdop,altitude,alt_unit,geoid_height,geoid_unit,checksum
    int commaIndex[14];
    int commaCount = 0;
    
    for (int i = 0; i < sentence.length() && commaCount < 14; i++) {
        if (sentence.charAt(i) == ',') {
            commaIndex[commaCount++] = i;
        }
    }
    
    if (commaCount >= 6) {
        String quality = sentence.substring(commaIndex[5] + 1, commaIndex[6]);
        if (quality.toInt() > 0) {
            currentData.isValid = true;
            
            // Parse latitude
            String latStr = sentence.substring(commaIndex[1] + 1, commaIndex[2]);
            String latDir = sentence.substring(commaIndex[2] + 1, commaIndex[3]);
            if (latStr.length() > 0) {
                currentData.latitude = convertDMSToDecimal(latStr, latDir);
            }
            
            // Parse longitude
            String lonStr = sentence.substring(commaIndex[3] + 1, commaIndex[4]);
            String lonDir = sentence.substring(commaIndex[4] + 1, commaIndex[5]);
            if (lonStr.length() > 0) {
                currentData.longitude = convertDMSToDecimal(lonStr, lonDir);
            }
            
            // Parse satellites
            if (commaCount >= 7) {
                String satStr = sentence.substring(commaIndex[6] + 1, commaIndex[7]);
                currentData.satellites = satStr.toInt();
            }
            
            // Parse HDOP
            if (commaCount >= 8) {
                String hdopStr = sentence.substring(commaIndex[7] + 1, commaIndex[8]);
                currentData.hdop = hdopStr.toFloat();
            }
            
            // Parse altitude
            if (commaCount >= 9) {
                String altStr = sentence.substring(commaIndex[8] + 1, commaIndex[9]);
                currentData.altitude = altStr.toFloat();
            }
        } else {
            currentData.isValid = false;
        }
    }
    return currentData.isValid;
}

bool Neo6mGPS::parseRMC(String sentence) {
    // Parse RMC sentence: $GPRMC,time,status,lat,lat_dir,lon,lon_dir,speed,course,date,checksum
    int commaIndex[12];
    int commaCount = 0;
    
    for (int i = 0; i < sentence.length() && commaCount < 12; i++) {
        if (sentence.charAt(i) == ',') {
            commaIndex[commaCount++] = i;
        }
    }
    
    if (commaCount >= 7) {
        String status = sentence.substring(commaIndex[1] + 1, commaIndex[2]);
        if (status == "A") { // Active (valid)
            // Parse speed (knots)
            String speedStr = sentence.substring(commaIndex[6] + 1, commaIndex[7]);
            if (speedStr.length() > 0) {
                currentData.speed = speedStr.toFloat() * 1.852; // Convert knots to km/h
            }
            
            // Parse timestamp
            String timeStr = sentence.substring(commaIndex[0] + 1, commaIndex[1]);
            String dateStr = sentence.substring(commaIndex[8] + 1, commaIndex[9]);
            currentData.timestamp = timeStr + " " + dateStr;
        }
    }
    return true;
}

float Neo6mGPS::convertDMSToDecimal(String dms, String direction) {
    if (dms.length() < 4) return 0.0;
    
    float degrees, minutes;
    if (dms.length() >= 7) { // Longitude format: dddmm.mmmm
        degrees = dms.substring(0, 3).toFloat();
        minutes = dms.substring(3).toFloat();
    } else { // Latitude format: ddmm.mmmm
        degrees = dms.substring(0, 2).toFloat();
        minutes = dms.substring(2).toFloat();
    }
    
    float decimal = degrees + (minutes / 60.0);
    
    if (direction == "S" || direction == "W") {
        decimal = -decimal;
    }
    
    return decimal;
}

bool Neo6mGPS::isLocationValid() {
    return currentData.isValid;
}

String Neo6mGPS::getLocationString() {
    if (currentData.isValid) {
        return "Lat: " + String(currentData.latitude, 6) + ", Lon: " + String(currentData.longitude, 6);
    }
    return "No GPS Fix";
}

float Neo6mGPS::getSpeed() {
    return currentData.speed;
}
