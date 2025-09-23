// BikeTrackerCore.h
// Core logic for BikeTracker system

#ifndef BIKETRACKERCORE_H
#define BIKETRACKERCORE_H

#include <Arduino.h>
#include "Neo6mGPS.h"
#include "Sim800L.h"
#include "ModeConfig.h"

enum TrackerState {
    TRACKER_INITIALIZING,
    TRACKER_STANDBY,
    TRACKER_TRACKING,
    TRACKER_ALERT,
    TRACKER_ERROR
};

enum AlertType {
    ALERT_NONE,
    ALERT_MOTION_DETECTED,
    ALERT_SPEED_EXCEEDED,
    ALERT_GEOFENCE_BREACH,
    ALERT_THEFT_DETECTED,
    ALERT_LOW_BATTERY,
    ALERT_SYSTEM_ERROR
};

struct TrackerStatus {
    TrackerState state;
    bool gpsFixed;
    bool gsmConnected;
    int batteryLevel;
    unsigned long uptime;
    int alertsCount;
    float lastSpeed;
    String lastLocation;
};

class BikeTrackerCore {
public:
    BikeTrackerCore(Neo6mGPS &gpsModule, Sim800L &gsmModule);
    
    // Core functions
    bool initialize();
    void update();
    TrackerStatus getStatus();
    
    // Security functions
    void armTracker();
    void disarmTracker();
    bool isArmed();
    
    // Alert functions
    void triggerAlert(AlertType type, const String &message = "");
    void clearAlerts();
    
    // Configuration
    void setEmergencyContact(const String &number);
    void setGeofenceCenter(float lat, float lon, float radius);
    void setSpeedLimit(float maxSpeed);
    
    // Tracking functions
    void requestLocationUpdate();
    String getCurrentLocation();
    void sendStatusSMS();
    
    // Testing functions (only available in testing mode)
    void runDiagnostics();
    void simulateAlert(AlertType type);
    
private:
    Neo6mGPS &gps;
    Sim800L &gsm;
    TrackerStatus status;
    
    // Configuration
    String emergencyContact;
    bool isTrackerArmed;
    float geofenceLat, geofenceLon, geofenceRadius;
    float speedLimit;
    
    // Timing
    unsigned long lastGPSUpdate;
    unsigned long lastSMSAlert;
    unsigned long lastStatusCheck;
    
    // State tracking
    bool motionDetected;
    float previousLat, previousLon;
    bool isInGeofence;
    
    // Internal functions
    void checkMotion();
    void checkSpeed();
    void checkGeofence();
    void updateGPS();
    void updateGSM();
    void processAlerts();
    float calculateDistance(float lat1, float lon1, float lat2, float lon2);
    String formatLocationMessage(const String &alertType = "");
    void blinkStatusLED(int times);
    void activateBuzzer(int duration);
};

#endif // BIKETRACKERCORE_H