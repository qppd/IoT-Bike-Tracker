# SIM800L Internet Connectivity Setup Guide

## Overview

This guide provides comprehensive instructions for setting up robust internet connectivity on your IoT Bike Tracker using the SIM800L GSM/GPRS module. The enhanced implementation includes automatic reconnection, connection monitoring, and extensive error handling.

## Hardware Requirements

### SIM800L Module
- SIM800L GSM/GPRS module
- Compatible SIM card with data plan
- Stable power supply (3.7V-4.2V, min 2A capability)
- GSM antenna

### Connections
Refer to the wiring diagram in `/Diagrams/Wiring.png` for proper connections.

## SIM Card Setup

### 1. SIM Card Requirements
- **Data Plan**: Ensure your SIM card has an active data plan
- **PIN Code**: Disable PIN code protection on the SIM card
- **Roaming**: Enable data roaming if needed for your location
- **Network Type**: 2G/GSM network compatibility required

### 2. Testing Your SIM Card
Before installation, test your SIM card in a phone to verify:
- Network registration works
- Data connectivity is functional
- APN settings are correct

## Configuration

### 1. Configure APN Settings

Edit `APIConfig.h` to match your mobile carrier:

```cpp
// === COMMON WORLDWIDE APNs ===
#define APN_NAME "internet"           // Generic (try this first)

// === US CARRIERS ===
// #define APN_NAME "vzwinternet"     // Verizon
// #define APN_NAME "phone"           // AT&T
// #define APN_NAME "fast.t-mobile.com" // T-Mobile

// === EUROPEAN CARRIERS ===
// #define APN_NAME "three.co.uk"     // Three UK
// #define APN_NAME "internet"        // Vodafone
// #define APN_NAME "orangeworld"     // Orange

// === ASIAN CARRIERS ===
// #define APN_NAME "airtelgprs.com"  // Airtel India
// #define APN_NAME "www"             // Jio India
```

### 2. Configure API Endpoint

Set your web API URL:

```cpp
#define WEB_API_URL "https://your-api.com/api/tracker"
#define DEVICE_ID "BIKE_TRACKER_001"
```

### 3. Adjust Connection Settings

Customize timeouts and retry behavior:

```cpp
#define GPRS_RETRY_ATTEMPTS 3        // Connection retry attempts
#define HTTP_RETRY_ATTEMPTS 3        // HTTP request retries
#define HTTP_UPDATE_INTERVAL 30000   // Regular updates (30 seconds)
#define CONNECTION_CHECK_INTERVAL 60000 // Connection health check (1 minute)
```

## Enhanced Features

### 1. Automatic Reconnection
- Automatically detects lost GPRS connections
- Attempts reconnection with exponential backoff
- Falls back to connection reset if needed

### 2. Connection Monitoring
- Continuous health monitoring
- Inactive connection detection
- Preventive connection maintenance

### 3. Robust HTTP Handling
- Automatic retry logic for failed requests
- Proper error code handling
- Enhanced JSON payload with metadata

### 4. Comprehensive Logging
- Detailed connection status reporting
- HTTP transaction logging
- Error diagnosis information

## Testing and Troubleshooting

### 1. Test Commands (Testing Mode)

Use the serial console to test connectivity:

```
CONNECT - Test internet connectivity
RESET   - Reset GPRS connection
API     - Send test data to API
STATUS  - Show system status
DIAG    - Run comprehensive diagnostics
```

### 2. Connection Test Output

Expected output for successful connection:

```
=== Internet Connectivity Test ===
GSM Network: Connected
Signal Strength: 18 (0-31, higher is better)
GPRS Status: Connected
Local IP: 10.64.64.64
Internet Test: SUCCESS - Internet accessible
Testing API connectivity...
HTTP POST result: SUCCESS
=== Test Complete ===
```

### 3. Common Issues and Solutions

#### Issue: "GSM Network: No network connection"
**Solutions:**
- Check antenna connection
- Verify SIM card is properly inserted
- Ensure SIM card is activated and has credit
- Check signal strength in your area

#### Issue: "GPRS Status: Disconnected"
**Solutions:**
- Verify APN settings for your carrier
- Check if data plan is active
- Try different APN from the list
- Ensure sufficient signal strength (>10)

#### Issue: "Internet Test: FAILED"
**Solutions:**
- Verify carrier data settings
- Check for network restrictions
- Try resetting the connection: `RESET` command
- Contact carrier about data connectivity

#### Issue: "HTTP POST result: FAILED"
**Solutions:**
- Verify API URL is correct and accessible
- Check if API server is running
- Ensure JSON format is accepted by your API
- Test API with external tools (Postman, curl)

## Power Management

### Critical Power Requirements
- **Stable Voltage**: 3.7V - 4.2V (4V recommended)
- **Current Capability**: Minimum 2A peak current
- **Power Supply**: Use quality power supply or battery
- **Brownout Protection**: Implement power monitoring

### Power-Related Issues
- Insufficient current causes connection drops
- Voltage fluctuations cause module resets
- Poor power supply affects signal quality

## API Integration

### Expected JSON Format

Your API will receive data in this format:

```json
{
    "deviceId": "BIKE_TRACKER_001",
    "latitude": 40.712800,
    "longitude": -74.006000,
    "timestamp": "1640995200000",
    "alertType": "",
    "signalStrength": 18,
    "localIP": "10.64.64.64",
    "imei": "123456789012345"
}
```

### API Response Requirements
- Return HTTP 200-299 for success
- JSON response format (optional but recommended):
```json
{
    "status": "success",
    "message": "Location data received",
    "deviceId": "BIKE_TRACKER_001"
}
```

## Advanced Configuration

### SSL/HTTPS Support
For HTTPS APIs (requires SIM800L with SSL support):

```cpp
#define SSL_ENABLED true
#define SSL_VERIFY_CERTIFICATE false  // For self-signed certificates
```

### Custom HTTP Headers
Add authentication or custom headers:

```cpp
// In your code:
gsm.setHTTPHeaders("Authorization: Bearer your-token");
```

### Data Usage Optimization
- Adjust `HTTP_UPDATE_INTERVAL` for less frequent updates
- Implement data compression if supported by your API
- Use SMS fallback for critical alerts

## Monitoring and Maintenance

### Real-time Monitoring
- Use `STATUS` command to check system health
- Monitor signal strength regularly
- Track connection uptime and failures

### Log Analysis
Enable detailed logging for troubleshooting:

```cpp
#define DETAILED_LOGGING_ENABLED true
```

### Performance Metrics
- Track HTTP success/failure rates
- Monitor GPRS reconnection frequency
- Analyze data usage patterns

## Security Considerations

### SIM Card Security
- Use SIM cards with secure profiles
- Monitor for unauthorized usage
- Implement data usage limits

### API Security
- Use HTTPS endpoints when possible
- Implement API authentication
- Validate device identifiers server-side

### Network Security
- Monitor for unusual network activity
- Implement rate limiting on API endpoints
- Use device certificates for enhanced security

## Carrier-Specific Notes

### Verizon
- APN: `vzwinternet`
- May require device activation
- Good rural coverage

### AT&T
- APN: `phone`
- Wide network coverage
- Reliable GPRS connectivity

### T-Mobile
- APN: `fast.t-mobile.com`
- Good urban coverage
- Fast data speeds

### International Carriers
- Test with `internet` APN first
- Contact carrier for IoT-specific plans
- Consider roaming agreements

## Troubleshooting Checklist

1. **Hardware Check**
   - [ ] Antenna properly connected
   - [ ] SIM card inserted correctly
   - [ ] Power supply adequate (2A+)
   - [ ] Voltage stable (3.7V-4.2V)

2. **SIM Card Check**
   - [ ] Data plan active
   - [ ] PIN code disabled
   - [ ] Network registration working
   - [ ] Sufficient credit/data allowance

3. **Configuration Check**
   - [ ] Correct APN for carrier
   - [ ] Valid API URL
   - [ ] Proper timeout settings
   - [ ] Debug logging enabled

4. **Network Check**
   - [ ] Signal strength >10
   - [ ] GPRS registration successful
   - [ ] Internet connectivity confirmed
   - [ ] API endpoint accessible

5. **Software Check**
   - [ ] Latest firmware version
   - [ ] No compilation errors
   - [ ] Debug output showing proper flow
   - [ ] Error handling working correctly

## Performance Optimization

### Connection Optimization
- Maintain persistent GPRS connections
- Use connection pooling when possible
- Implement smart retry algorithms

### Data Optimization
- Compress JSON payloads
- Batch multiple updates when appropriate
- Use efficient data formats

### Power Optimization
- Power down module during inactive periods
- Use sleep modes when supported
- Optimize update frequencies

## Support and Updates

For issues not covered in this guide:
1. Check the project's GitHub issues
2. Review Web_API_Guide.md for server-side setup
3. Consult SIM800L documentation
4. Contact your mobile carrier for APN support

Regular updates to this implementation will include:
- Additional carrier APN configurations
- Enhanced error handling
- Performance improvements
- Security enhancements