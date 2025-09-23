// BikeTrackerCore.cpp
// Implementation of BikeTracker core logic

#include "BikeTrackerCore.h"
#include "PinConfig.h"
#include "APIConfig.h"
#include <math.h>

BikeTrackerCore::BikeTrackerCore(Neo6mGPS &gpsModule, Sim800L &gsmModule) 
    : gps(gpsModule), gsm(gsmModule) {
    
    // Initialize status
    status.state = TRACKER_INITIALIZING;
    status.gpsFixed = false;
    status.gsmConnected = false;
    status.batteryLevel = 100;
    status.uptime = 0;
    status.alertsCount = 0;
    status.lastSpeed = 0.0;
    status.lastLocation = "Unknown";
    
    // Initialize configuration
    emergencyContact = EMERGENCY_CONTACT;
    isTrackerArmed = false;
    geofenceLat = 0.0;
    geofenceLon = 0.0;
    geofenceRadius = 1000.0; // 1km default
    speedLimit = MAX_SPEED_THRESHOLD;
    webAPIUrl = "";
    deviceId = "";
    apnName = "";
    httpEnabled = false;
    
    // Initialize timing
    lastGPSUpdate = 0;
    lastSMSAlert = 0;
    lastStatusCheck = 0;
    lastHTTPUpdate = 0;
    
    // Initialize state tracking
    motionDetected = false;
    previousLat = 0.0;
    previousLon = 0.0;
    isInGeofence = true;
}

bool BikeTrackerCore::initialize() {
    DEBUG_PRINTLN("Initializing BikeTracker...");
    
    // Initialize pins
    pinMode(LED_STATUS_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Signal initialization start
    blinkStatusLED(3);
    
    // Initialize GPS
    DEBUG_PRINTLN("Initializing GPS...");
    gps.begin(9600);
    delay(2000);
    
    // Initialize GSM
    DEBUG_PRINTLN("Initializing GSM...");
    gsm.begin(9600);
    
    // Wait for GSM initialization
    for (int i = 0; i < 10; i++) {
        if (gsm.initialize()) {
            status.gsmConnected = true;
            DEBUG_PRINTLN("GSM initialized successfully");
            break;
        }
        DEBUG_PRINT("GSM init attempt ");
        DEBUG_PRINTLN(i + 1);
        delay(2000);
    }
    
    if (!status.gsmConnected) {
        DEBUG_PRINTLN("GSM initialization failed");
        status.state = TRACKER_ERROR;
        return false;
    }
    
    // Wait for GPS fix
    DEBUG_PRINTLN("Waiting for GPS fix...");
    unsigned long gpsStartTime = millis();
    while (millis() - gpsStartTime < 60000) { // 1 minute timeout
        updateGPS();
        if (status.gpsFixed) {
            DEBUG_PRINTLN("GPS fix acquired");
            break;
        }
        blinkStatusLED(1);
        delay(1000);
    }
    
    if (status.gpsFixed) {
        status.state = TRACKER_STANDBY;
        DEBUG_PRINTLN("BikeTracker initialized successfully");
        
        // Send initialization SMS in testing mode
        if (CURRENT_MODE == MODE_TESTING) {
            gsm.sendLocationSMS(emergencyContact, getCurrentLocation(), "System Initialized");
        }
        
        // Signal successful initialization
        blinkStatusLED(5);
        activateBuzzer(100);
        
        return true;
    } else {
        status.state = TRACKER_ERROR;
        DEBUG_PRINTLN("GPS fix not acquired - continuing without GPS");
        
        // Signal GPS error but continue
        for (int i = 0; i < 3; i++) {
            blinkStatusLED(2);
            delay(500);
        }
        
        return true; // Continue operation without GPS for now
    }
}

void BikeTrackerCore::update() {
    status.uptime = millis();
    
    // Update modules
    updateGPS();
    updateGSM();
    
    // Enhanced connection monitoring
    if (httpEnabled && CONNECTION_MONITORING_ENABLED) {
        static unsigned long lastConnectionCheck = 0;
        if (millis() - lastConnectionCheck > CONNECTION_CHECK_INTERVAL) {
            lastConnectionCheck = millis();
            
            // Check GPRS connection health
            if (!gsm.isGPRSConnected()) {
                DEBUG_PRINTLN("GPRS connection lost, attempting reconnection...");
                if (gsm.reconnectGPRS()) {
                    DEBUG_PRINTLN("GPRS reconnection successful");
                } else {
                    DEBUG_PRINTLN("GPRS reconnection failed");
                }
            }
            
            // Check for inactive connections
            unsigned long inactiveTime = millis() - gsm.getLastDataActivity();
            if (inactiveTime > CONNECTION_TIMEOUT) {
                DEBUG_PRINTLN("Connection inactive for too long, resetting...");
                gsm.resetConnection();
            }
        }
    }
    
    // Check system state
    if (millis() - lastStatusCheck > 5000) { // Check every 5 seconds
        lastStatusCheck = millis();
        
        // Battery monitoring disabled - no voltage sensor available
        // TODO: Implement when voltage/current sensor is added
        // status.batteryLevel = readBatteryVoltage(); // Implement with actual sensor
        
        // Simulate battery level for display purposes only
        status.batteryLevel = 100; // Always show full battery without sensor
        
        // Battery alerts disabled without sensor
        // if (status.batteryLevel < 20) {
        //     triggerAlert(ALERT_LOW_BATTERY, "Battery critically low: " + String(status.batteryLevel) + "%");
        // }
    }
    
    // Security monitoring (only when armed)
    if (isTrackerArmed) {
        checkMotion();
        checkSpeed();
        checkGeofence();
    }
    
    // Process any pending alerts
    processAlerts();
    
    // Send location updates to web API (if enabled)
    if (httpEnabled && status.gpsFixed) {
        sendLocationToAPI();
    }
    
    // Update status LED
    if (status.state == TRACKER_STANDBY || status.state == TRACKER_TRACKING) {
        digitalWrite(LED_STATUS_PIN, (millis() / 1000) % 2); // Slow blink when normal
    } else if (status.state == TRACKER_ALERT) {
        digitalWrite(LED_STATUS_PIN, (millis() / 200) % 2); // Fast blink when alert
    } else {
        digitalWrite(LED_STATUS_PIN, LOW); // Off when error
    }
}

void BikeTrackerCore::updateGPS() {
    if (millis() - lastGPSUpdate > GPS_UPDATE_INTERVAL) {
        lastGPSUpdate = millis();
        
        GPSData gpsData = gps.parseGPSData();
        
        if (gpsData.isValid) {
            if (!status.gpsFixed) {
                status.gpsFixed = true;
                DEBUG_PRINTLN("GPS fix acquired");
                blinkStatusLED(2);
            }
            
            status.lastSpeed = gpsData.speed;
            status.lastLocation = String(gpsData.latitude, 6) + "," + String(gpsData.longitude, 6);
            
            // Store previous position for motion detection
            if (previousLat != 0.0 && previousLon != 0.0) {
                float distance = calculateDistance(previousLat, previousLon, gpsData.latitude, gpsData.longitude);
                if (distance > MOTION_THRESHOLD) {
                    motionDetected = true;
                }
            }
            
            previousLat = gpsData.latitude;
            previousLon = gpsData.longitude;
            
            DEBUG_PRINT("GPS: ");
            DEBUG_PRINT(status.lastLocation);
            DEBUG_PRINT(" Speed: ");
            DEBUG_PRINTLN(status.lastSpeed);
            
        } else {
            if (status.gpsFixed) {
                DEBUG_PRINTLN("GPS fix lost");
                status.gpsFixed = false;
                // Trigger alert if GPS was lost for extended period
                static unsigned long gpsLostTime = 0;
                if (gpsLostTime == 0) {
                    gpsLostTime = millis();
                } else if (millis() - gpsLostTime > 300000) { // 5 minutes without GPS
                    triggerAlert(ALERT_GPS_LOST, "GPS signal lost for extended period");
                    gpsLostTime = 0; // Reset to prevent spam
                }
            }
        }
    }
}

void BikeTrackerCore::updateGSM() {
    // Check GSM status periodically
    static unsigned long lastGSMCheck = 0;
    static unsigned long gsmLostTime = 0;
    
    if (millis() - lastGSMCheck > 30000) { // Check every 30 seconds
        lastGSMCheck = millis();
        
        bool previousConnection = status.gsmConnected;
        status.gsmConnected = gsm.isNetworkConnected();
        
        if (!status.gsmConnected) {
            DEBUG_PRINTLN("GSM network lost - attempting reconnection");
            gsm.initialize();
            
            // Track how long GSM has been lost
            if (previousConnection) {
                gsmLostTime = millis(); // Just lost connection
            } else if (millis() - gsmLostTime > 600000) { // 10 minutes without GSM
                // Can't send alert via SMS if GSM is down, but log it
                DEBUG_PRINTLN("ALERT: GSM connection lost for extended period");
                gsmLostTime = millis(); // Reset timer to prevent spam
            }
        } else {
            gsmLostTime = 0; // Reset timer when connection is restored
        }
    }
}

void BikeTrackerCore::checkMotion() {
    if (motionDetected && status.state == TRACKER_STANDBY) {
        DEBUG_PRINTLN("Motion detected while armed!");
        status.state = TRACKER_TRACKING;
        triggerAlert(ALERT_MOTION_DETECTED, "Unauthorized movement detected");
        motionDetected = false; // Reset flag
    }
}

void BikeTrackerCore::checkSpeed() {
    if (status.lastSpeed > speedLimit) {
        DEBUG_PRINTLN("Speed limit exceeded!");
        triggerAlert(ALERT_SPEED_EXCEEDED, "Speed limit exceeded: " + String(status.lastSpeed, 1) + " km/h");
    }
}

void BikeTrackerCore::checkGeofence() {
    if (status.gpsFixed && geofenceLat != 0.0 && geofenceLon != 0.0) {
        float distance = calculateDistance(previousLat, previousLon, geofenceLat, geofenceLon);
        bool currentlyInFence = (distance <= geofenceRadius);
        
        if (isInGeofence && !currentlyInFence) {
            DEBUG_PRINTLN("Geofence breach detected!");
            triggerAlert(ALERT_GEOFENCE_BREACH, "Vehicle left safe area");
            isInGeofence = false;
        } else if (!isInGeofence && currentlyInFence) {
            DEBUG_PRINTLN("Vehicle returned to safe area");
            isInGeofence = true;
        }
    }
}

void BikeTrackerCore::triggerAlert(AlertType type, const String &message) {
    if (millis() - lastSMSAlert < SMS_ALERT_INTERVAL) {
        return; // Prevent SMS spam
    }
    
    status.state = TRACKER_ALERT;
    status.alertsCount++;
    lastSMSAlert = millis();
    
    String alertTypeStr;
    switch (type) {
        case ALERT_MOTION_DETECTED: alertTypeStr = "MOTION ALERT"; break;
        case ALERT_SPEED_EXCEEDED: alertTypeStr = "SPEED ALERT"; break;
        case ALERT_GEOFENCE_BREACH: alertTypeStr = "GEOFENCE ALERT"; break;
        case ALERT_SYSTEM_ERROR: alertTypeStr = "SYSTEM ERROR"; break;
        case ALERT_GPS_LOST: alertTypeStr = "GPS LOST"; break;
        case ALERT_GSM_LOST: alertTypeStr = "GSM LOST"; break;
        default: alertTypeStr = "GENERAL ALERT"; break;
    }
    
    DEBUG_PRINT("ALERT: ");
    DEBUG_PRINTLN(alertTypeStr);
    DEBUG_PRINTLN(message);
    
    // Send SMS alert
    if (status.gsmConnected) {
        String fullMessage = alertTypeStr;
        if (message.length() > 0) {
            fullMessage += "\n" + message;
        }
        fullMessage += "\nLocation: " + getCurrentLocation();
        fullMessage += "\nTime: " + String(millis() / 1000) + "s uptime";
        
        gsm.sendSMS(emergencyContact, fullMessage);
    }
    
    // Send alert to web API
    sendAlertToAPI(type, message);
    
    // Activate buzzer
    activateBuzzer(1000);
    
    // Return to tracking state after alert
    delay(2000);
    if (isTrackerArmed) {
        status.state = TRACKER_TRACKING;
    } else {
        status.state = TRACKER_STANDBY;
    }
}

void BikeTrackerCore::armTracker() {
    isTrackerArmed = true;
    status.state = TRACKER_STANDBY;
    DEBUG_PRINTLN("Tracker ARMED");
    
    // Reset motion detection baseline
    motionDetected = false;
    
    // Send confirmation
    if (CURRENT_MODE == MODE_TESTING && status.gsmConnected) {
        gsm.sendLocationSMS(emergencyContact, getCurrentLocation(), "Tracker Armed");
    }
    
    // Audio/visual confirmation
    blinkStatusLED(3);
    activateBuzzer(200);
}

void BikeTrackerCore::disarmTracker() {
    isTrackerArmed = false;
    status.state = TRACKER_STANDBY;
    DEBUG_PRINTLN("Tracker DISARMED");
    
    // Send confirmation
    if (CURRENT_MODE == MODE_TESTING && status.gsmConnected) {
        gsm.sendLocationSMS(emergencyContact, getCurrentLocation(), "Tracker Disarmed");
    }
    
    // Audio/visual confirmation
    blinkStatusLED(1);
    activateBuzzer(100);
}

bool BikeTrackerCore::isArmed() {
    return isTrackerArmed;
}

void BikeTrackerCore::setEmergencyContact(const String &number) {
    emergencyContact = number;
    DEBUG_PRINT("Emergency contact set to: ");
    DEBUG_PRINTLN(emergencyContact);
}

void BikeTrackerCore::setGeofenceCenter(float lat, float lon, float radius) {
    geofenceLat = lat;
    geofenceLon = lon;
    geofenceRadius = radius;
    isInGeofence = true; // Assume we start inside the fence
    
    DEBUG_PRINT("Geofence set: ");
    DEBUG_PRINT(String(lat, 6));
    DEBUG_PRINT(", ");
    DEBUG_PRINT(String(lon, 6));
    DEBUG_PRINT(" radius: ");
    DEBUG_PRINTLN(radius);
}

void BikeTrackerCore::setSpeedLimit(float maxSpeed) {
    speedLimit = maxSpeed;
    DEBUG_PRINT("Speed limit set to: ");
    DEBUG_PRINTLN(speedLimit);
}

TrackerStatus BikeTrackerCore::getStatus() {
    return status;
}

String BikeTrackerCore::getCurrentLocation() {
    if (status.gpsFixed) {
        return status.lastLocation;
    } else {
        return "GPS fix not available";
    }
}

void BikeTrackerCore::sendStatusSMS() {
    if (!status.gsmConnected) return;
    
    String statusMsg = "BikeTracker Status:\n";
    statusMsg += "State: ";
    switch (status.state) {
        case TRACKER_STANDBY: statusMsg += "Standby"; break;
        case TRACKER_TRACKING: statusMsg += "Tracking"; break;
        case TRACKER_ALERT: statusMsg += "Alert"; break;
        case TRACKER_ERROR: statusMsg += "Error"; break;
        default: statusMsg += "Unknown"; break;
    }
    statusMsg += "\nArmed: " + String(isTrackerArmed ? "Yes" : "No");
    statusMsg += "\nGPS: " + String(status.gpsFixed ? "Fixed" : "No Fix");
    statusMsg += "\nLocation: " + getCurrentLocation();
    statusMsg += "\nSpeed: " + String(status.lastSpeed, 1) + " km/h";
    statusMsg += "\nBattery: " + String(status.batteryLevel) + "%";
    statusMsg += "\nUptime: " + String(status.uptime / 1000) + "s";
    statusMsg += "\nAlerts: " + String(status.alertsCount);
    
    gsm.sendSMS(emergencyContact, statusMsg);
}

void BikeTrackerCore::runDiagnostics() {
    if (CURRENT_MODE != MODE_TESTING) return;
    
    DEBUG_PRINTLN("=== DIAGNOSTICS START ===");
    
    // Test GPS
    DEBUG_PRINT("GPS Status: ");
    DEBUG_PRINTLN(status.gpsFixed ? "OK" : "FAIL");
    DEBUG_PRINT("GPS Location: ");
    DEBUG_PRINTLN(getCurrentLocation());
    
    // Test GSM
    DEBUG_PRINT("GSM Status: ");
    DEBUG_PRINTLN(status.gsmConnected ? "OK" : "FAIL");
    DEBUG_PRINT("Signal Strength: ");
    DEBUG_PRINTLN(gsm.getSignalStrength());
    
    // Test components
    DEBUG_PRINTLN("Testing LED...");
    blinkStatusLED(5);
    
    DEBUG_PRINTLN("Testing Buzzer...");
    activateBuzzer(500);
    
    DEBUG_PRINTLN("=== DIAGNOSTICS END ===");
}

void BikeTrackerCore::simulateAlert(AlertType type) {
    if (CURRENT_MODE != MODE_TESTING) return;
    
    DEBUG_PRINTLN("Simulating alert for testing...");
    triggerAlert(type, "Test alert - ignore");
}

void BikeTrackerCore::processAlerts() {
    // Process any pending alerts or state changes
    // This function can be expanded for more complex alert handling
}

float BikeTrackerCore::calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    // Haversine formula for calculating distance between two GPS coordinates
    const float R = 6371000; // Earth's radius in meters
    
    float dLat = (lat2 - lat1) * PI / 180.0;
    float dLon = (lon2 - lon1) * PI / 180.0;
    
    float a = sin(dLat / 2) * sin(dLat / 2) +
              cos(lat1 * PI / 180.0) * cos(lat2 * PI / 180.0) *
              sin(dLon / 2) * sin(dLon / 2);
    
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return R * c; // Distance in meters
}

void BikeTrackerCore::blinkStatusLED(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_STATUS_PIN, HIGH);
        delay(200);
        digitalWrite(LED_STATUS_PIN, LOW);
        delay(200);
    }
}

void BikeTrackerCore::activateBuzzer(int duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
}

void BikeTrackerCore::setWebAPI(const String &url, const String &deviceIdentifier, const String &apn) {
    webAPIUrl = url;
    deviceId = deviceIdentifier;
    apnName = apn;
    httpEnabled = (url.length() > 0 && deviceId.length() > 0 && apn.length() > 0);
    
    if (httpEnabled) {
        DEBUG_PRINTLN("Web API configured:");
        DEBUG_PRINT("URL: ");
        DEBUG_PRINTLN(webAPIUrl);
        DEBUG_PRINT("Device ID: ");
        DEBUG_PRINTLN(deviceId);
        DEBUG_PRINT("APN: ");
        DEBUG_PRINTLN(apnName);
        
        // Initialize GPRS connection with enhanced settings
        if (status.gsmConnected) {
            DEBUG_PRINTLN("Initializing GPRS connection...");
            
            // Enable automatic time sync for accurate timestamps
            gsm.enableAutoTimeSync();
            
            // Initialize GPRS with retry logic
            bool gprsSuccess = false;
            for (int attempt = 0; attempt < GPRS_RETRY_ATTEMPTS; attempt++) {
                DEBUG_PRINT("GPRS connection attempt ");
                DEBUG_PRINTLN(attempt + 1);
                
                gprsSuccess = gsm.initializeGPRS(apnName, APN_USERNAME, APN_PASSWORD);
                
                if (gprsSuccess) {
                    DEBUG_PRINTLN("GPRS initialization: SUCCESS");
                    
                    // Test internet connectivity
                    if (gsm.checkInternetConnectivity()) {
                        DEBUG_PRINTLN("Internet connectivity: VERIFIED");
                        DEBUG_PRINT("Local IP: ");
                        DEBUG_PRINTLN(gsm.getLocalIP());
                    } else {
                        DEBUG_PRINTLN("Internet connectivity: FAILED");
                    }
                    break;
                } else {
                    DEBUG_PRINTLN("GPRS initialization: FAILED");
                    if (attempt < GPRS_RETRY_ATTEMPTS - 1) {
                        delay(GPRS_RETRY_DELAY);
                    }
                }
            }
            
            if (!gprsSuccess) {
                DEBUG_PRINTLN("GPRS initialization failed after all attempts");
                httpEnabled = false; // Disable HTTP if GPRS fails
            }
        }
    }
}

void BikeTrackerCore::sendLocationToAPI() {
    if (!httpEnabled || !status.gsmConnected || !status.gpsFixed) {
        return;
    }
    
    // Check if enough time has passed since last HTTP update
    if (millis() - lastHTTPUpdate < HTTP_UPDATE_INTERVAL) {
        return;
    }
    
    // Check connection health
    if (CONNECTION_MONITORING_ENABLED) {
        if (!gsm.maintainConnection()) {
            DEBUG_PRINTLN("Failed to maintain GPRS connection");
            return;
        }
    }
    
    lastHTTPUpdate = millis();
    
    DEBUG_PRINTLN("Sending location to web API...");
    
    // Extract latitude and longitude from current location
    int commaIndex = status.lastLocation.indexOf(',');
    if (commaIndex > 0) {
        float lat = status.lastLocation.substring(0, commaIndex).toFloat();
        float lon = status.lastLocation.substring(commaIndex + 1).toFloat();
        
        // Validate coordinates
        if (lat == 0.0 && lon == 0.0) {
            DEBUG_PRINTLN("Invalid GPS coordinates, skipping API call");
            return;
        }
        
        bool success = false;
        
        // Retry logic for API calls
        for (int attempt = 0; attempt < HTTP_RETRY_ATTEMPTS; attempt++) {
            if (DETAILED_LOGGING_ENABLED && attempt > 0) {
                DEBUG_PRINT("HTTP retry attempt ");
                DEBUG_PRINTLN(attempt + 1);
            }
            
            success = gsm.sendLocationHTTP(webAPIUrl, deviceId, lat, lon, "");
            
            if (success) {
                break;
            }
            
            // Wait before retry
            if (attempt < HTTP_RETRY_ATTEMPTS - 1) {
                delay(HTTP_RETRY_DELAY);
            }
        }
        
        DEBUG_PRINT("HTTP POST result: ");
        DEBUG_PRINTLN(success ? "SUCCESS" : "FAILED");
        
        if (success) {
            blinkStatusLED(1); // Quick blink on successful upload
        } else {
            DEBUG_PRINTLN("All HTTP attempts failed");
            
            // Try to reset connection on persistent failures
            if (AUTO_RECONNECT_ENABLED) {
                DEBUG_PRINTLN("Attempting connection reset...");
                gsm.resetConnection();
            }
        }
    }
}

void BikeTrackerCore::sendAlertToAPI(AlertType type, const String &message) {
    if (!httpEnabled || !status.gsmConnected) {
        return;
    }
    
    String alertTypeStr = "";
    switch (type) {
        case ALERT_MOTION_DETECTED:
            alertTypeStr = "MOTION_DETECTED";
            break;
        case ALERT_SPEED_EXCEEDED:
            alertTypeStr = "SPEED_EXCEEDED";
            break;
        case ALERT_GEOFENCE_BREACH:
            alertTypeStr = "GEOFENCE_BREACH";
            break;
        case ALERT_SYSTEM_ERROR:
            alertTypeStr = "SYSTEM_ERROR";
            break;
        case ALERT_GPS_LOST:
            alertTypeStr = "GPS_LOST";
            break;
        case ALERT_GSM_LOST:
            alertTypeStr = "GSM_LOST";
            break;
        default:
            alertTypeStr = "UNKNOWN";
            break;
    }
    
    DEBUG_PRINT("Sending alert to web API: ");
    DEBUG_PRINTLN(alertTypeStr);
    
    // For alerts, ensure immediate connectivity
    if (!gsm.maintainConnection()) {
        DEBUG_PRINTLN("Failed to maintain connection for alert");
        return;
    }
    
    // Extract latitude and longitude from current location
    int commaIndex = status.lastLocation.indexOf(',');
    if (commaIndex > 0) {
        float lat = status.lastLocation.substring(0, commaIndex).toFloat();
        float lon = status.lastLocation.substring(commaIndex + 1).toFloat();
        
        bool success = false;
        
        // For alerts, use more aggressive retry logic
        for (int attempt = 0; attempt < (HTTP_RETRY_ATTEMPTS + 2); attempt++) {
            if (attempt > 0) {
                DEBUG_PRINT("Alert retry attempt ");
                DEBUG_PRINTLN(attempt + 1);
            }
            
            success = gsm.sendLocationHTTP(webAPIUrl, deviceId, lat, lon, alertTypeStr);
            
            if (success) {
                break;
            }
            
            // Shorter retry delay for alerts
            if (attempt < HTTP_RETRY_ATTEMPTS + 1) {
                delay(HTTP_RETRY_DELAY / 2);
            }
        }
        
        DEBUG_PRINT("Alert HTTP POST result: ");
        DEBUG_PRINTLN(success ? "SUCCESS" : "FAILED");
        
        if (!success) {
            DEBUG_PRINTLN("CRITICAL: Alert failed to send to API");
        }
    }
}