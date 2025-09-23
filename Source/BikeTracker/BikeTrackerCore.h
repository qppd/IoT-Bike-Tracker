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
    ALERT_MOTION_DETECTED,     // ✅ Works with GPS
    ALERT_SPEED_EXCEEDED,      // ✅ Works with GPS
    ALERT_GEOFENCE_BREACH,     // ✅ Works with GPS
    ALERT_SYSTEM_ERROR,        // ✅ Works with software monitoring
    // ALERT_THEFT_DETECTED,   // ❌ Requires additional sensors (accelerometer, etc.)
    // ALERT_LOW_BATTERY,       // ❌ Requires voltage/current sensor
    ALERT_GPS_LOST,            // ✅ New: GPS fix lost alert
    ALERT_GSM_LOST             // ✅ New: GSM connection lost alert
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
    void setWebAPI(const String &url, const String &deviceId, const String &apn);
    
    // Tracking functions
    void requestLocationUpdate();
    String getCurrentLocation();
    void sendStatusSMS();
    void sendLocationToAPI();
    
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
    String webAPIUrl;
    String deviceId;
    String apnName;
    bool httpEnabled;
    
    // Timing
    unsigned long lastGPSUpdate;
    unsigned long lastSMSAlert;
    unsigned long lastStatusCheck;
    unsigned long lastHTTPUpdate;
    
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
    void sendAlertToAPI(AlertType type, const String &message);
    float calculateDistance(float lat1, float lon1, float lat2, float lon2);
    String formatLocationMessage(const String &alertType = "");
    void blinkStatusLED(int times);
    void activateBuzzer(int duration);
};

#endif // BIKETRACKERCORE_H