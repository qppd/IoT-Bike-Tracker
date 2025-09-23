
// BikeTracker.ino
// Main Arduino sketch for IoT Bike Tracker
// Supports both Testing/Development and Production modes

#include <Arduino.h>
#include <SoftwareSerial.h>

// Project headers
#include "ModeConfig.h"
#include "PinConfig.h"
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
        
        #if CURRENT_MODE == MODE_TESTING
            Serial.println("\n=== TESTING MODE COMMANDS ===");
            Serial.println("ARM     - Arm the tracker");
            Serial.println("DISARM  - Disarm the tracker");
            Serial.println("STATUS  - Get current status");
            Serial.println("DIAG    - Run diagnostics");
            Serial.println("ALERT   - Simulate motion alert");
            Serial.println("SPEED   - Simulate speed alert");
            Serial.println("LOCATE  - Send location SMS");
            Serial.println("HELP    - Show this menu");
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
            
        } else if (serialCommand == "LOCATE") {
            tracker.sendStatusSMS();
            Serial.println("Location SMS sent");
            
        } else if (serialCommand == "HELP") {
            Serial.println("\n=== TESTING MODE COMMANDS ===");
            Serial.println("ARM     - Arm the tracker");
            Serial.println("DISARM  - Disarm the tracker");
            Serial.println("STATUS  - Get current status");
            Serial.println("DIAG    - Run diagnostics");
            Serial.println("ALERT   - Simulate motion alert");
            Serial.println("SPEED   - Simulate speed alert");
            Serial.println("LOCATE  - Send location SMS");
            Serial.println("HELP    - Show this menu");
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
    Serial.print("km/h Bat:");
    Serial.print(status.batteryLevel);
    Serial.println("%");
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
    
    Serial.print("Battery: ");
    Serial.print(status.batteryLevel);
    Serial.println("%");
    
    Serial.print("Total Alerts: ");
    Serial.println(status.alertsCount);
    
    Serial.print("Free Memory: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    
    Serial.println("================================\n");
}
