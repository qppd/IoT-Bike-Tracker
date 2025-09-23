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

// === PHILIPPINES (PH) CARRIERS ===
// Uncomment your Philippine carrier's APN:
// #define APN_NAME "internet"              // Globe Telecom
// #define APN_NAME "smartlte"              // Smart Communications (LTE)
// #define APN_NAME "smart"                 // Smart Communications (3G/4G)
// #define APN_NAME "internet.dito.ph"      // Dito Telecommunity
// #define APN_NAME "internet"              // Sun Cellular (merged with Smart)
// #define APN_NAME "internet"              // TNT (Talk 'N Text - Smart brand)
// #define APN_NAME "myglobe"               // Globe Postpaid
// #define APN_NAME "http.globe.com.ph"     // Globe HTTP proxy APN

// Philippine APN Settings with credentials (if required)
// For Globe with username/password authentication:
// #define APN_NAME "internet"
// #define APN_USERNAME "globe"
// #define APN_PASSWORD "globe"

// For Smart with username/password authentication:
// #define APN_NAME "smartlte"
// #define APN_USERNAME "smart"
// #define APN_PASSWORD "smart"

// Default PH Configuration (Generic for most PH carriers)
#define APN_NAME "internet"              // Works with most PH carriers

// Optional: APN username and password (usually not needed for most carriers)
#define APN_USERNAME ""
#define APN_PASSWORD ""

// Advanced GPRS Settings
#define GPRS_CONNECTION_TIMEOUT 30000    // Timeout for GPRS connection (ms)
#define GPRS_RETRY_ATTEMPTS 3            // Number of retry attempts for failed connections
#define GPRS_RETRY_DELAY 5000           // Delay between retry attempts (ms)

// Philippines-specific GPRS optimizations
#define PH_NETWORK_REGISTRATION_TIMEOUT 60000  // PH networks may take longer to register
#define PH_SIGNAL_QUALITY_THRESHOLD 10         // Minimum signal quality for PH networks
#define PH_NETWORK_SCAN_INTERVAL 120000        // Scan for better networks every 2 minutes
#define PH_ROAMING_ENABLED false               // Disable roaming by default for cost control

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

// Philippines-specific data transmission settings
#define PH_DATA_COMPRESSION_ENABLED true    // Enable data compression for PH networks
#define PH_BATCH_UPDATES_ENABLED true      // Batch multiple updates to save data costs
#define PH_MAX_BATCH_SIZE 5                // Maximum updates per batch for PH networks
#define PH_OFFLINE_STORAGE_ENABLED true    // Store data offline during poor connectivity
#define PH_MAX_OFFLINE_RECORDS 50          // Maximum records to store offline

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