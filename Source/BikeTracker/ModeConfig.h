// ModeConfig.h
// Configuration for BikeTracker modes

#ifndef MODECONFIG_H
#define MODECONFIG_H

// Operating modes
#define MODE_TESTING 0
#define MODE_PRODUCTION 1

// Set the current mode here (0 = Testing, 1 = Production)
#define CURRENT_MODE MODE_TESTING

// Configuration based on mode
#if CURRENT_MODE == MODE_TESTING
    #define DEBUG_ENABLED true
    #define GPS_UPDATE_INTERVAL 5000      // 5 seconds for testing
    #define SMS_ALERT_INTERVAL 30000      // 30 seconds for testing
    #define MOTION_THRESHOLD 5            // Lower threshold for testing
    #define MAX_SPEED_THRESHOLD 10        // km/h for testing alerts
#else
    #define DEBUG_ENABLED false
    #define GPS_UPDATE_INTERVAL 60000     // 1 minute for production
    #define SMS_ALERT_INTERVAL 300000     // 5 minutes for production
    #define MOTION_THRESHOLD 15           // Higher threshold for production
    #define MAX_SPEED_THRESHOLD 80        // km/h for production alerts
#endif

// Emergency contact (modify for production use)
#if CURRENT_MODE == MODE_TESTING
    #define EMERGENCY_CONTACT "+1234567890"  // Test number
#else
    #define EMERGENCY_CONTACT "+1987654321"  // Real emergency contact
#endif

// Debug macro
#if DEBUG_ENABLED
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

#endif // MODECONFIG_H