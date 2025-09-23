// APIConfig.h
// Configuration for Web API connectivity

#ifndef APICONFIG_H
#define APICONFIG_H

// Web API Configuration
// Modify these values according to your web API setup

// Your web API endpoint URL
#define WEB_API_URL "https://your-api.com/api/tracker"

// Unique device identifier (change this for each device)
#define DEVICE_ID "BIKE_TRACKER_001"

// APN settings for your mobile carrier
// Choose the appropriate APN for your carrier:

// === COMMON WORLDWIDE APNs ===
// Generic/Default
#define APN_NAME "internet"

// === US CARRIERS ===
// Uncomment your carrier's APN:
// #define APN_NAME "vzwinternet"           // Verizon
// #define APN_NAME "phone"                 // AT&T
// #define APN_NAME "fast.t-mobile.com"     // T-Mobile
// #define APN_NAME "wholesale"             // T-Mobile MVNO

// === EUROPEAN CARRIERS ===
// #define APN_NAME "three.co.uk"           // Three UK
// #define APN_NAME "internet"              // Vodafone (most countries)
// #define APN_NAME "orangeworld"           // Orange
// #define APN_NAME "web.telecom.it"        // TIM Italy
// #define APN_NAME "internet.t-mobile.de"  // T-Mobile Germany

// === ASIAN CARRIERS ===
// #define APN_NAME "internet"              // Generic Asia
// #define APN_NAME "airtelgprs.com"        // Airtel India
// #define APN_NAME "www"                   // Jio India

// Optional: APN username and password (usually not needed for most carriers)
#define APN_USERNAME ""
#define APN_PASSWORD ""

// Advanced GPRS Settings
#define GPRS_CONNECTION_TIMEOUT 30000    // Timeout for GPRS connection (ms)
#define GPRS_RETRY_ATTEMPTS 3            // Number of retry attempts for failed connections
#define GPRS_RETRY_DELAY 5000           // Delay between retry attempts (ms)

// HTTP request settings
#define HTTP_TIMEOUT 30000               // HTTP request timeout (ms)
#define HTTP_RETRY_ATTEMPTS 3            // Number of HTTP retry attempts
#define HTTP_RETRY_DELAY 2000           // Delay between HTTP retries (ms)

// Connection monitoring
#define CONNECTION_CHECK_INTERVAL 60000  // Check connection health every 60 seconds
#define CONNECTION_TIMEOUT 300000        // Reset connection if inactive for 5 minutes

// Data transmission intervals
#define HTTP_UPDATE_INTERVAL 30000       // Regular location updates (30 seconds)
#define HTTP_ALERT_IMMEDIATE true        // Send alerts immediately (true/false)

// Enable/disable features
#define HTTP_ENABLED true                // Enable web API functionality
#define AUTO_RECONNECT_ENABLED true      // Enable automatic reconnection
#define CONNECTION_MONITORING_ENABLED true // Enable connection health monitoring
#define DETAILED_LOGGING_ENABLED true    // Enable detailed HTTP logging

// SSL/TLS Settings (for HTTPS URLs)
#define SSL_ENABLED false                // Enable SSL (requires SIM800L with SSL support)
#define SSL_VERIFY_CERTIFICATE false     // Verify SSL certificates

// JSON payload settings
#define INCLUDE_IMEI_IN_PAYLOAD true     // Include IMEI in JSON
#define INCLUDE_LOCAL_IP_IN_PAYLOAD true // Include local IP in JSON
#define INCLUDE_TIMESTAMP_IN_PAYLOAD true // Include timestamp in JSON

#endif // APICONFIG_H