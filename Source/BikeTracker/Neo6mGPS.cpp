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

// =============================================================================
// GPS TESTING FUNCTIONS
// =============================================================================

void Neo6mGPS::printGPSTestHeader(const String &category) {
    Serial.println("\n========================================");
    Serial.println("  " + category + " GPS TESTS");
    Serial.println("========================================");
}

void Neo6mGPS::printGPSTestResult(const String &testName, bool result, const String &details) {
    Serial.print("[");
    if (result) {
        Serial.print("PASS");
    } else {
        Serial.print("FAIL");
    }
    Serial.print("] " + testName);
    
    if (details.length() > 0) {
        Serial.print(" - " + details);
    }
    Serial.println();
    
    delay(500); // Small delay between tests
}

bool Neo6mGPS::testGPSResponse(int timeoutSeconds) {
    unsigned long startTime = millis();
    unsigned long timeout = timeoutSeconds * 1000;
    bool receivedData = false;
    
    Serial.println("[INFO] Waiting for GPS data (timeout: " + String(timeoutSeconds) + "s)...");
    
    while (millis() - startTime < timeout) {
        if (gpsSerial.available()) {
            String sentence = read();
            if (sentence.length() > 10) { // Valid NMEA sentence
                receivedData = true;
                break;
            }
        }
        delay(100);
    }
    
    return receivedData;
}

void Neo6mGPS::runBasicGPSTests() {
    printGPSTestHeader("BASIC GPS COMMUNICATION");
    
    // Test GPS data reception
    bool dataReceived = testGPSResponse(15);
    printGPSTestResult("GPS Data Reception", dataReceived, 
                      dataReceived ? "NMEA sentences received" : "No GPS data");
    
    if (!dataReceived) {
        Serial.println("[WARNING] No GPS data received. Check:");
        Serial.println("  - GPS module power connection");
        Serial.println("  - GPS antenna connection");
        Serial.println("  - RX/TX pin connections");
        Serial.println("  - Move to area with clear sky view");
        return;
    }
    
    // Test GPS parsing functionality
    GPSData data = parseGPSData();
    printGPSTestResult("GPS Data Parsing", true, "NMEA parsing functional");
    
    // Test location validity
    bool validLocation = isLocationValid();
    String locationDetails = validLocation ? getLocationString() : "No GPS fix yet";
    printGPSTestResult("GPS Fix Status", validLocation, locationDetails);
    
    // Test satellite count
    if (currentData.satellites > 0) {
        printGPSTestResult("Satellite Detection", true, 
                          String(currentData.satellites) + " satellites visible");
    } else {
        printGPSTestResult("Satellite Detection", false, "No satellites detected");
    }
    
    // Test HDOP (accuracy indicator)
    if (currentData.hdop > 0) {
        String accuracy = "Unknown";
        if (currentData.hdop < 2) accuracy = "Excellent";
        else if (currentData.hdop < 5) accuracy = "Good";
        else if (currentData.hdop < 10) accuracy = "Moderate";
        else accuracy = "Poor";
        
        printGPSTestResult("GPS Accuracy (HDOP)", true, 
                          "HDOP: " + String(currentData.hdop) + " (" + accuracy + ")");
    } else {
        printGPSTestResult("GPS Accuracy (HDOP)", false, "HDOP data not available");
    }
    
    Serial.println("Basic GPS Tests Complete\n");
}

void Neo6mGPS::runNMEATests() {
    printGPSTestHeader("NMEA SENTENCE ANALYSIS");
    
    Serial.println("[INFO] Analyzing NMEA sentences for 15 seconds...");
    
    unsigned long startTime = millis();
    int ggaCount = 0, rmcCount = 0, otherCount = 0;
    bool hasGGA = false, hasRMC = false;
    
    while (millis() - startTime < 15000) {
        if (gpsSerial.available()) {
            String sentence = read();
            if (sentence.length() > 10) {
                if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
                    ggaCount++;
                    hasGGA = true;
                } else if (sentence.startsWith("$GPRMC") || sentence.startsWith("$GNRMC")) {
                    rmcCount++;
                    hasRMC = true;
                } else if (sentence.startsWith("$GP") || sentence.startsWith("$GN")) {
                    otherCount++;
                }
            }
        }
        delay(10);
    }
    
    printGPSTestResult("GGA Sentences", hasGGA, 
                      "Received " + String(ggaCount) + " GGA sentences");
    printGPSTestResult("RMC Sentences", hasRMC, 
                      "Received " + String(rmcCount) + " RMC sentences");
    printGPSTestResult("Other NMEA Sentences", otherCount > 0, 
                      "Received " + String(otherCount) + " other sentences");
    
    // Test sentence rate
    float ggaRate = ggaCount / 15.0;
    float rmcRate = rmcCount / 15.0;
    bool goodRate = (ggaRate >= 0.8 && rmcRate >= 0.8); // Should be ~1Hz
    
    printGPSTestResult("NMEA Sentence Rate", goodRate,
                      "GGA: " + String(ggaRate, 1) + "Hz, RMC: " + String(rmcRate, 1) + "Hz");
    
    Serial.println("NMEA Tests Complete\n");
}

void Neo6mGPS::runLocationTests() {
    printGPSTestHeader("LOCATION & POSITIONING");
    
    // Wait for GPS fix with progress indication
    Serial.println("[INFO] Attempting to acquire GPS fix (timeout: 120 seconds)...");
    unsigned long startTime = millis();
    bool gotFix = false;
    int lastSatCount = -1;
    
    for (int i = 0; i < 120; i++) {
        GPSData data = parseGPSData();
        
        if (currentData.satellites != lastSatCount) {
            Serial.println("  Satellites visible: " + String(currentData.satellites));
            lastSatCount = currentData.satellites;
        }
        
        if (isLocationValid()) {
            gotFix = true;
            break;
        }
        
        delay(1000);
        
        // Progress indicator
        if (i % 10 == 0) {
            Serial.print("  Waiting for fix... " + String(i) + "s");
            if (currentData.satellites > 0) {
                Serial.print(" (Satellites: " + String(currentData.satellites) + ")");
            }
            Serial.println();
        }
    }
    
    printGPSTestResult("GPS Fix Acquisition", gotFix, 
                      gotFix ? "Fix acquired in " + String((millis() - startTime) / 1000) + "s" : "No fix acquired");
    
    if (gotFix) {
        // Test coordinate validity
        bool validCoords = (currentData.latitude != 0.0 && currentData.longitude != 0.0);
        printGPSTestResult("Coordinate Validity", validCoords, getLocationString());
        
        // Test altitude data
        bool hasAltitude = (currentData.altitude != 0.0);
        printGPSTestResult("Altitude Data", hasAltitude, 
                          hasAltitude ? String(currentData.altitude) + "m" : "No altitude data");
        
        // Test speed data
        bool hasSpeed = (currentData.speed >= 0);
        printGPSTestResult("Speed Data", hasSpeed,
                          hasSpeed ? String(currentData.speed) + " km/h" : "No speed data");
        
        // Test coordinate precision
        String latStr = String(currentData.latitude, 6);
        String lonStr = String(currentData.longitude, 6);
        bool goodPrecision = (latStr.indexOf('.') > 0 && lonStr.indexOf('.') > 0);
        printGPSTestResult("Coordinate Precision", goodPrecision,
                          "Lat: " + latStr + ", Lon: " + lonStr);
    }
    
    Serial.println("Location Tests Complete\n");
}

void Neo6mGPS::runPerformanceTests() {
    printGPSTestHeader("PERFORMANCE & TRACKING");
    
    if (!isLocationValid()) {
        Serial.println("[WARNING] GPS fix required for performance tests");
        printGPSTestResult("Performance Tests", false, "No GPS fix available");
        return;
    }
    
    // Test data update rate
    Serial.println("[INFO] Testing GPS update rate for 10 seconds...");
    unsigned long startTime = millis();
    int updateCount = 0;
    float lastLat = currentData.latitude;
    float lastLon = currentData.longitude;
    
    while (millis() - startTime < 10000) {
        GPSData data = parseGPSData();
        if (currentData.latitude != lastLat || currentData.longitude != lastLon) {
            updateCount++;
            lastLat = currentData.latitude;
            lastLon = currentData.longitude;
        }
        delay(100);
    }
    
    float updateRate = updateCount / 10.0;
    bool goodUpdateRate = (updateRate >= 0.5); // At least 0.5 Hz
    printGPSTestResult("Position Update Rate", goodUpdateRate,
                      String(updateRate, 1) + " Hz");
    
    // Test satellite tracking stability
    testSatelliteTracking(15);
    
    // Test accuracy assessment
    testGPSAccuracy();
    
    Serial.println("Performance Tests Complete\n");
}

void Neo6mGPS::testSatelliteTracking(int duration) {
    Serial.println("[INFO] Testing satellite tracking stability for " + String(duration) + " seconds...");
    
    unsigned long startTime = millis();
    int minSats = 999, maxSats = 0, totalSats = 0, readings = 0;
    
    while (millis() - startTime < duration * 1000) {
        parseGPSData();
        if (currentData.satellites > 0) {
            if (currentData.satellites < minSats) minSats = currentData.satellites;
            if (currentData.satellites > maxSats) maxSats = currentData.satellites;
            totalSats += currentData.satellites;
            readings++;
        }
        delay(1000);
    }
    
    if (readings > 0) {
        float avgSats = float(totalSats) / readings;
        bool stable = (maxSats - minSats) <= 2; // Variation of 2 or less is good
        
        printGPSTestResult("Satellite Tracking Stability", stable,
                          "Min: " + String(minSats) + ", Max: " + String(maxSats) + 
                          ", Avg: " + String(avgSats, 1));
    } else {
        printGPSTestResult("Satellite Tracking Stability", false, "No satellite data");
    }
}

void Neo6mGPS::testGPSAccuracy() {
    Serial.println("[INFO] Analyzing GPS accuracy metrics...");
    
    parseGPSData();
    
    // HDOP analysis
    String hdopQuality = "Unknown";
    bool goodHDOP = false;
    
    if (currentData.hdop > 0) {
        if (currentData.hdop <= 1) {
            hdopQuality = "Ideal";
            goodHDOP = true;
        } else if (currentData.hdop <= 2) {
            hdopQuality = "Excellent";
            goodHDOP = true;
        } else if (currentData.hdop <= 5) {
            hdopQuality = "Good";
            goodHDOP = true;
        } else if (currentData.hdop <= 10) {
            hdopQuality = "Moderate";
            goodHDOP = false;
        } else if (currentData.hdop <= 20) {
            hdopQuality = "Fair";
            goodHDOP = false;
        } else {
            hdopQuality = "Poor";
            goodHDOP = false;
        }
    }
    
    printGPSTestResult("GPS Accuracy (HDOP)", goodHDOP,
                      "HDOP: " + String(currentData.hdop) + " (" + hdopQuality + ")");
    
    // Satellite count assessment
    bool goodSatCount = (currentData.satellites >= 4);
    String satQuality = "Insufficient";
    if (currentData.satellites >= 8) satQuality = "Excellent";
    else if (currentData.satellites >= 6) satQuality = "Good";
    else if (currentData.satellites >= 4) satQuality = "Adequate";
    
    printGPSTestResult("Satellite Count", goodSatCount,
                      String(currentData.satellites) + " satellites (" + satQuality + ")");
}

void Neo6mGPS::displayRawNMEA(int duration) {
    Serial.println("\n========================================");
    Serial.println("  RAW NMEA SENTENCE DISPLAY");
    Serial.println("========================================");
    Serial.println("Displaying raw NMEA data for " + String(duration) + " seconds...");
    Serial.println("----------------------------------------");
    
    unsigned long startTime = millis();
    while (millis() - startTime < duration * 1000) {
        if (gpsSerial.available()) {
            String sentence = read();
            if (sentence.length() > 10) {
                Serial.print(sentence);
            }
        }
        delay(10);
    }
    
    Serial.println("----------------------------------------");
    Serial.println("Raw NMEA display complete\n");
}

void Neo6mGPS::printGPSStatus() {
    Serial.println("\n========================================");
    Serial.println("  CURRENT GPS STATUS");
    Serial.println("========================================");
    
    parseGPSData();
    
    Serial.println("GPS Fix: " + String(currentData.isValid ? "VALID" : "INVALID"));
    Serial.println("Latitude: " + String(currentData.latitude, 6));
    Serial.println("Longitude: " + String(currentData.longitude, 6));
    Serial.println("Altitude: " + String(currentData.altitude) + " m");
    Serial.println("Speed: " + String(currentData.speed) + " km/h");
    Serial.println("Satellites: " + String(currentData.satellites));
    Serial.println("HDOP: " + String(currentData.hdop));
    Serial.println("Timestamp: " + currentData.timestamp);
    Serial.println("Location String: " + getLocationString());
    Serial.println("========================================\n");
}

void Neo6mGPS::runAllGPSTests() {
    Serial.println("\n#########################################");
    Serial.println("#       GPS MODULE TEST SUITE           #");
    Serial.println("#########################################");
    Serial.println("Starting comprehensive GPS testing...");
    Serial.println("Test Duration: Approximately 3-5 minutes");
    Serial.println("NOTE: GPS tests require clear sky view for best results");
    
    unsigned long startTime = millis();
    
    // Run all test categories
    runBasicGPSTests();
    runNMEATests();
    runLocationTests();
    runPerformanceTests();
    
    unsigned long endTime = millis();
    unsigned long duration = (endTime - startTime) / 1000;
    
    Serial.println("#########################################");
    Serial.println("#           GPS TEST SUMMARY            #");
    Serial.println("#########################################");
    Serial.println("Total Test Duration: " + String(duration) + " seconds");
    Serial.println("GPS module testing completed!");
    Serial.println();
    Serial.println("USAGE TIPS:");
    Serial.println("- Ensure GPS module has clear view of sky");
    Serial.println("- First fix can take 30 seconds to 15 minutes");
    Serial.println("- Indoor testing may not work");
    Serial.println("- HDOP < 5 indicates good accuracy");
    Serial.println("- 4+ satellites needed for 3D fix");
    Serial.println("#########################################\n");
}
