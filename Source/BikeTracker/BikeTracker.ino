
// BikeTracker.ino
// Main Arduino sketch for IoT Bike Tracker
// Supports both Testing/Development and Production modes

#include <Arduino.h>
#include <SoftwareSerial.h>

// Project headers
#include "ModeConfig.h"
#include "PinConfig.h"
#include "APIConfig.h"
#include "Neo6mGPS.h"
#include "Sim800L.h"
#include "BikeTrackerCore.h"

// Hardware objects
SoftwareSerial SerialGPS(GPS_RX_PIN, GPS_TX_PIN);
SoftwareSerial SerialGSM(GSM_RX_PIN, GSM_TX_PIN);
Neo6mGPS gps(SerialGPS);
Sim800L gsm(SerialGSM);
BikeTrackerCore tracker(gps, gsm);

// Timing variables
unsigned long lastSerialOutput = 0;
unsigned long lastStatusReport = 0;

// Command processing for testing mode
String serialCommand = "";
bool commandReady = false;

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    delay(2000);
    
    // Print startup information
    Serial.println("=====================================");
    Serial.println("     IoT BIKE TRACKER SYSTEM");
    Serial.println("=====================================");
    Serial.print("Mode: ");
    #if CURRENT_MODE == MODE_TESTING
        Serial.println("TESTING/DEVELOPMENT");
        Serial.println("Debug output: ENABLED");
        Serial.println("Serial commands: ENABLED");
        Serial.println("Accelerated timing: ENABLED");
    #else
        Serial.println("PRODUCTION");
        Serial.println("Debug output: DISABLED");
        Serial.println("Serial commands: DISABLED");
        Serial.println("Normal timing: ENABLED");
    #endif
    Serial.println("=====================================");
    
    // Initialize hardware components
    DEBUG_PRINTLN("Initializing hardware components...");
    SerialGPS.begin(9600);
    SerialGSM.begin(9600);
    
    // Initialize tracker core
    DEBUG_PRINTLN("Initializing BikeTracker core...");
    if (tracker.initialize()) {
        DEBUG_PRINTLN("BikeTracker initialization successful!");
        
        // Configure Web API (modify values in APIConfig.h)
        #if HTTP_ENABLED
            tracker.setWebAPI(WEB_API_URL, DEVICE_ID, APN_NAME);
        #endif
        
        #if CURRENT_MODE == MODE_TESTING
            Serial.println("\n=== TESTING MODE COMMANDS ===");
            Serial.println("ARM       - Arm the tracker");
            Serial.println("DISARM    - Disarm the tracker");
            Serial.println("STATUS    - Get current status");
            Serial.println("DIAG      - Run diagnostics");
            Serial.println("ALERT     - Simulate motion alert");
            Serial.println("SPEED     - Simulate speed alert");
            Serial.println("FENCE     - Simulate geofence breach");
            Serial.println("LOCATE    - Send location SMS");
            Serial.println("API       - Send location to API");
            Serial.println("SLEEP     - Enter sleep mode (5 min)");
            Serial.println("DEEPSLEEP - Enter deep sleep (30 min)");
            Serial.println("LOWPOWER  - Toggle low power mode");
            Serial.println("");
            Serial.println("=== AT COMMAND TESTING ===");
            Serial.println("ATTEST    - Run all AT command tests");
            Serial.println("ATBASIC   - Test basic AT commands");
            Serial.println("ATNETWORK - Test network commands");
            Serial.println("ATSMS     - Test SMS commands");
            Serial.println("ATGPRS    - Test GPRS commands");
            Serial.println("ATHTTP    - Test HTTP commands");
            Serial.println("");
            Serial.println("HELP      - Show this menu");
            Serial.println("NOTE: Power management fully implemented");
            Serial.println("=============================\n");
        #endif
    } else {
        DEBUG_PRINTLN("BikeTracker initialization FAILED!");
        Serial.println("System will continue with limited functionality...");
    }
    
    DEBUG_PRINTLN("Setup complete. Starting main loop...");
}

void loop() {
    // Update tracker core (this handles all the main logic)
    tracker.update();
    
    // Handle mode-specific operations
    #if CURRENT_MODE == MODE_TESTING
        handleTestingMode();
    #else
        handleProductionMode();
    #endif
    
    // Physical button functionality removed - use serial commands instead
    handleButton();
    
    // Periodic status reporting
    if (millis() - lastStatusReport > 300000) { // Every 5 minutes
        lastStatusReport = millis();
        
        #if CURRENT_MODE == MODE_TESTING
            printDetailedStatus();
        #endif
        
        // Send status SMS in production mode (less frequent)
        #if CURRENT_MODE == MODE_PRODUCTION
            static int statusCount = 0;
            statusCount++;
            if (statusCount >= 12) { // Every hour (5min * 12 = 60min)
                tracker.sendStatusSMS();
                statusCount = 0;
            }
        #endif
    }
    
    // Small delay to prevent overwhelming the system
    delay(100);
}

void handleTestingMode() {
    // Process serial commands for testing
    processSerialCommands();
    
    // Frequent status output for debugging
    if (millis() - lastSerialOutput > 10000) { // Every 10 seconds
        lastSerialOutput = millis();
        printQuickStatus();
    }
}

void handleProductionMode() {
    // In production mode, we run quietly with minimal serial output
    // Main functionality is handled by tracker.update()
    
    // Only critical information is sent to serial (if connected)
    static unsigned long lastProductionLog = 0;
    if (millis() - lastProductionLog > 60000) { // Every minute
        lastProductionLog = millis();
        
        TrackerStatus status = tracker.getStatus();
        if (status.state == TRACKER_ALERT || status.state == TRACKER_ERROR) {
            Serial.print("ALERT: State=");
            Serial.print(status.state);
            Serial.print(" GPS=");
            Serial.print(status.gpsFixed ? "OK" : "FAIL");
            Serial.print(" GSM=");
            Serial.println(status.gsmConnected ? "OK" : "FAIL");
        }
    }
}

void processSerialCommands() {
    // Read serial input
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (serialCommand.length() > 0) {
                commandReady = true;
            }
        } else {
            serialCommand += c;
        }
    }
    
    // Process commands
    if (commandReady) {
        serialCommand.toUpperCase();
        serialCommand.trim();
        
        DEBUG_PRINT("Command received: ");
        DEBUG_PRINTLN(serialCommand);
        
        if (serialCommand == "ARM") {
            tracker.armTracker();
            Serial.println("Tracker ARMED");
            
        } else if (serialCommand == "DISARM") {
            tracker.disarmTracker();
            Serial.println("Tracker DISARMED");
            
        } else if (serialCommand == "STATUS") {
            printDetailedStatus();
            
        } else if (serialCommand == "DIAG") {
            tracker.runDiagnostics();
            
        } else if (serialCommand == "ALERT") {
            tracker.simulateAlert(ALERT_MOTION_DETECTED);
            Serial.println("Motion alert simulated");
            
        } else if (serialCommand == "SPEED") {
            tracker.simulateAlert(ALERT_SPEED_EXCEEDED);
            Serial.println("Speed alert simulated");
            
        } else if (serialCommand == "FENCE") {
            tracker.simulateAlert(ALERT_GEOFENCE_BREACH);
            Serial.println("Geofence breach alert simulated");
            
        } else if (serialCommand == "LOCATE") {
            tracker.sendStatusSMS();
            Serial.println("Location SMS sent");
            
        } else if (serialCommand == "API") {
            tracker.sendLocationToAPI();
            Serial.println("Location sent to API");
            
        } else if (serialCommand == "CONNECT") {
            Serial.println("Testing internet connectivity...");
            testInternetConnectivity();
            
        } else if (serialCommand == "RESET") {
            Serial.println("Resetting GPRS connection...");
            gsm.resetConnection();
            Serial.println("Connection reset complete");
            
        } else if (serialCommand == "SLEEP") {
            Serial.println("Entering sleep mode for 5 minutes...");
            tracker.enterSleepMode(300000); // 5 minutes
            
        } else if (serialCommand == "DEEPSLEEP") {
            Serial.println("Entering deep sleep for 30 minutes...");
            tracker.enterDeepSleep(1800000); // 30 minutes
            
        } else if (serialCommand == "LOWPOWER") {
            bool currentMode = tracker.isInLowPowerMode();
            tracker.enableLowPowerMode(!currentMode);
            Serial.print("Low power mode: ");
            Serial.println(!currentMode ? "ENABLED" : "DISABLED");
            
        } else if (serialCommand == "WAKE") {
            Serial.println("Wake up requested");
            tracker.wakeFromSleep();
            
        } else if (serialCommand == "ATTEST") {
            Serial.println("Running comprehensive AT command tests...");
            gsm.runAllATTests();
            
        } else if (serialCommand == "ATBASIC") {
            Serial.println("Running basic AT command tests...");
            gsm.runBasicATTests();
            
        } else if (serialCommand == "ATNETWORK") {
            Serial.println("Running network AT command tests...");
            gsm.runNetworkTests();
            
        } else if (serialCommand == "ATSMS") {
            Serial.println("Running SMS AT command tests...");
            gsm.runSMSTests();
            
        } else if (serialCommand == "ATGPRS") {
            Serial.println("Running GPRS AT command tests...");
            gsm.runGPRSTests();
            
        } else if (serialCommand == "ATHTTP") {
            Serial.println("Running HTTP AT command tests...");
            gsm.runHTTPTests();
            
        } else if (serialCommand == "HELP") {
            Serial.println("\n=== TESTING MODE COMMANDS ===");
            Serial.println("ARM       - Arm the tracker");
            Serial.println("DISARM    - Disarm the tracker");
            Serial.println("STATUS    - Get current status");
            Serial.println("DIAG      - Run diagnostics");
            Serial.println("ALERT     - Simulate motion alert");
            Serial.println("SPEED     - Simulate speed alert");
            Serial.println("FENCE     - Simulate geofence breach");
            Serial.println("LOCATE    - Send location SMS");
            Serial.println("API       - Send location to API");
            Serial.println("CONNECT   - Test internet connectivity");
            Serial.println("RESET     - Reset GPRS connection");
            Serial.println("SLEEP     - Enter sleep mode (5 min)");
            Serial.println("DEEPSLEEP - Enter deep sleep (30 min)");
            Serial.println("LOWPOWER  - Toggle low power mode");
            Serial.println("WAKE      - Wake from sleep");
            Serial.println("");
            Serial.println("=== AT COMMAND TESTING ===");
            Serial.println("ATTEST    - Run all AT command tests");
            Serial.println("ATBASIC   - Test basic AT commands");
            Serial.println("ATNETWORK - Test network commands");
            Serial.println("ATSMS     - Test SMS commands");
            Serial.println("ATGPRS    - Test GPRS commands");
            Serial.println("ATHTTP    - Test HTTP commands");
            Serial.println("");
            Serial.println("HELP      - Show this menu");
            Serial.println("NOTE: Power management fully implemented");
            Serial.println("=============================\n");
            
        } else if (serialCommand.length() > 0) {
            Serial.print("Unknown command: ");
            Serial.println(serialCommand);
            Serial.println("Type HELP for available commands");
        }
        
        // Clear command
        serialCommand = "";
        commandReady = false;
    }
}

void handleButton() {
    // Button functionality removed - use serial commands in testing mode
    // or implement alternative control method if needed
}

void printQuickStatus() {
    TrackerStatus status = tracker.getStatus();
    
    Serial.print("[");
    Serial.print(millis() / 1000);
    Serial.print("s] State:");
    
    switch (status.state) {
        case TRACKER_INITIALIZING: Serial.print("INIT"); break;
        case TRACKER_STANDBY: Serial.print("STANDBY"); break;
        case TRACKER_TRACKING: Serial.print("TRACK"); break;
        case TRACKER_ALERT: Serial.print("ALERT"); break;
        case TRACKER_ERROR: Serial.print("ERROR"); break;
    }
    
    Serial.print(" GPS:");
    Serial.print(status.gpsFixed ? "OK" : "NO");
    Serial.print(" GSM:");
    Serial.print(status.gsmConnected ? "OK" : "NO");
    Serial.print(" Armed:");
    Serial.print(tracker.isArmed() ? "Y" : "N");
    Serial.print(" Speed:");
    Serial.print(status.lastSpeed, 1);
    Serial.print("km/h Power:");
    Serial.println(tracker.isInLowPowerMode() ? "LOW" : "NORM");
}

void printDetailedStatus() {
    TrackerStatus status = tracker.getStatus();
    
    Serial.println("\n======== DETAILED STATUS ========");
    Serial.print("Uptime: ");
    Serial.print(status.uptime / 1000);
    Serial.println(" seconds");
    
    Serial.print("State: ");
    switch (status.state) {
        case TRACKER_INITIALIZING: Serial.println("Initializing"); break;
        case TRACKER_STANDBY: Serial.println("Standby"); break;
        case TRACKER_TRACKING: Serial.println("Tracking"); break;
        case TRACKER_ALERT: Serial.println("Alert"); break;
        case TRACKER_ERROR: Serial.println("Error"); break;
    }
    
    Serial.print("Armed: ");
    Serial.println(tracker.isArmed() ? "YES" : "NO");
    
    Serial.print("GPS Status: ");
    Serial.println(status.gpsFixed ? "Fixed" : "No Fix");
    
    Serial.print("Location: ");
    Serial.println(tracker.getCurrentLocation());
    
    Serial.print("Speed: ");
    Serial.print(status.lastSpeed, 1);
    Serial.println(" km/h");
    
    Serial.print("GSM Status: ");
    Serial.println(status.gsmConnected ? "Connected" : "Disconnected");
    
    Serial.print("Power Mode: ");
    Serial.println(tracker.isInLowPowerMode() ? "Low Power" : "Normal");
    
    Serial.print("Total Alerts: ");
    Serial.println(status.alertsCount);
    
    Serial.print("Free Memory: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    
    Serial.println("================================\n");
}

#if CURRENT_MODE == MODE_TESTING
void testInternetConnectivity() {
    Serial.println("=== Internet Connectivity Test ===");
    
    // Check GSM status
    Serial.print("GSM Network: ");
    if (gsm.isNetworkConnected()) {
        Serial.println("Connected");
        
        // Check signal strength
        int signal = gsm.getSignalStrength();
        Serial.print("Signal Strength: ");
        Serial.print(signal);
        Serial.println(" (0-31, higher is better)");
        
        // Check GPRS connection
        Serial.print("GPRS Status: ");
        if (gsm.isGPRSConnected()) {
            Serial.println("Connected");
            
            // Show local IP
            String localIP = gsm.getLocalIP();
            Serial.print("Local IP: ");
            Serial.println(localIP);
            
            // Test internet connectivity
            Serial.print("Internet Test: ");
            if (gsm.checkInternetConnectivity()) {
                Serial.println("SUCCESS - Internet accessible");
                
                // Test API connectivity if configured
                if (strlen(WEB_API_URL) > 0 && strcmp(WEB_API_URL, "https://your-api.com/api/tracker") != 0) {
                    Serial.println("Testing API connectivity...");
                    tracker.sendLocationToAPI();
                } else {
                    Serial.println("API URL not configured (check APIConfig.h)");
                }
            } else {
                Serial.println("FAILED - No internet access");
            }
        } else {
            Serial.println("Disconnected");
            Serial.println("Attempting GPRS reconnection...");
            
            if (gsm.reconnectGPRS()) {
                Serial.println("GPRS reconnection successful");
            } else {
                Serial.println("GPRS reconnection failed");
            }
        }
    } else {
        Serial.println("No network connection");
    }
    
    Serial.println("=== Test Complete ===");
}
#endif
