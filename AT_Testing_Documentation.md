# Hardware Testing Documentation

## Overview
This document describes the comprehensive hardware testing functionality added to the IoT Bike Tracker system. The testing suite allows you to verify both GSM and GPS module functionality, troubleshoot connectivity issues, and validate hardware behavior.

## Features Added

### 1. Integrated Testing in BikeTracker.ino
The main BikeTracker system now includes built-in hardware testing when running in `MODE_TESTING`. New commands have been added to the existing serial command interface for both GSM and GPS modules.

### 2. GSM Module Testing (SIM800L)
Comprehensive AT command testing for GSM functionality including network registration, SMS, GPRS, and HTTP capabilities.

### 3. GPS Module Testing (Neo-6M)
Complete GPS testing suite including NMEA sentence analysis, satellite tracking, location accuracy, and performance metrics.

## Available Test Commands

### GSM/AT Command Testing
- `ATTEST` - Run all AT command tests (comprehensive suite)
- `ATBASIC` - Test basic AT commands (communication, info, signal)
- `ATNETWORK` - Test network registration and operator commands
- `ATSMS` - Test SMS functionality commands
- `ATGPRS` - Test GPRS/bearer connection commands
- `ATHTTP` - Test HTTP service commands

### GPS Module Testing  
- `GPSTEST` - Run all GPS tests (comprehensive suite)
- `GPSBASIC` - Test basic GPS communication and data reception
- `GPSNMEA` - Test NMEA sentence parsing and analysis
- `GPSLOCATION` - Test GPS fix acquisition and location accuracy
- `GPSPERF` - Test GPS performance and tracking stability
- `GPSSTATUS` - Display current GPS status and data
- `GPSRAW` - Display raw NMEA sentences for 10 seconds

### Test Categories

#### 1. Basic Communication Tests
- **AT** - Basic communication test
- **ATE0** - Disable echo
- **AT+GMI** - Get manufacturer information
- **AT+GMM** - Get model information
- **AT+GMR** - Get firmware version
- **AT+GSN** - Get IMEI number
- **AT+CPIN?** - Check SIM card status
- **AT+CSQ** - Signal quality measurement
- **AT+CBC** - Battery status (if supported)

#### 2. Network & Registration Tests
- **AT+CREG?** - Network registration status
- **AT+CGREG?** - GPRS registration status
- **AT+COPS?** - Current operator information
- **AT+COPS=?** - Scan available operators (slow)
- **AT+CCLK?** - Network time
- **AT+CIPGSMLOC** - Cell tower location

#### 3. SMS Functionality Tests
- **AT+CMGF=1** - Set SMS text mode
- **AT+CPMS?** - Check SMS storage
- **AT+CSCA?** - Get SMS center number
- **AT+CSCS="GSM"** - Set character set
- **AT+CMGL="ALL"** - List all SMS messages
- **AT+CNMI** - Configure SMS notifications

#### 4. GPRS Connectivity Tests
- **AT+CGATT?** - GPRS attachment status
- **AT+CGDCONT?** - PDP context settings
- **AT+SAPBR=2,1** - Bearer profile status
- **AT+SAPBR=3,1** - Configure bearer profile
- **AT+SAPBR=1,1** - Open bearer connection
- **AT+CGPADDR** - Get GPRS IP address

#### 5. HTTP Functionality Tests
- **AT+HTTPINIT** - Initialize HTTP service
- **AT+HTTPPARA** - Set HTTP parameters
- **AT+HTTPSSL** - Configure HTTPS
- **AT+HTTPTERM** - Terminate HTTP service

## GPS Test Categories

#### 1. Basic GPS Communication Tests
- **Data Reception** - Test GPS module communication
- **NMEA Parsing** - Verify sentence parsing functionality
- **GPS Fix Status** - Check current location fix status
- **Satellite Detection** - Count visible satellites
- **GPS Accuracy (HDOP)** - Horizontal dilution of precision

#### 2. NMEA Sentence Analysis Tests
- **GGA Sentences** - Position fix data analysis
- **RMC Sentences** - Recommended minimum data analysis
- **Other NMEA** - Additional sentence types
- **Sentence Rate** - Update frequency verification
- **Data Integrity** - Checksum and format validation

#### 3. Location & Positioning Tests
- **GPS Fix Acquisition** - Time to first fix measurement
- **Coordinate Validity** - Latitude/longitude validation
- **Altitude Data** - Elevation information accuracy
- **Speed Data** - Velocity calculation verification
- **Coordinate Precision** - Decimal precision analysis

#### 4. Performance & Tracking Tests
- **Position Update Rate** - Location refresh frequency
- **Satellite Tracking Stability** - Satellite count consistency
- **GPS Accuracy Assessment** - HDOP and satellite analysis
- **Signal Quality** - Reception strength evaluation

#### 5. Raw Data Analysis
- **NMEA Display** - Real-time sentence monitoring
- **Signal Diagnostics** - Reception quality assessment
- **Timing Analysis** - Update interval verification

## Usage Instructions

### Using Integrated Testing (Recommended)

1. **Set Testing Mode**: Ensure `CURRENT_MODE` is set to `MODE_TESTING` in `ModeConfig.h`
2. **Upload and Connect**: Upload BikeTracker.ino and open Serial Monitor (9600 baud)
3. **Run GSM Tests**: Type AT test commands:
   ```
   ATTEST      // Run all GSM tests (3-5 minutes)
   ATBASIC     // Quick basic tests (30 seconds)
   ATNETWORK   // Network tests (1-2 minutes)
   ```
4. **Run GPS Tests**: Type GPS test commands:
   ```
   GPSTEST     // Run all GPS tests (3-5 minutes)
   GPSBASIC    // Quick GPS communication test (30 seconds)
   GPSLOCATION // GPS fix and location test (2 minutes)
   ```

### GPS Testing Requirements

**Important Notes for GPS Testing:**
- GPS tests require **clear sky view** for best results
- **Indoor testing may not work** due to weak satellite signals
- **First GPS fix** can take 30 seconds to 15 minutes (cold start)
- **Warm start** (after recent use) typically takes 30-60 seconds
- **Move to outdoor location** with minimal obstructions for testing

### Using Standalone Tester

1. **Upload ATCommandTester.ino** to your Arduino
2. **Connect Serial Monitor** at 9600 baud
3. **Automatic Testing**: Tests run automatically on startup
4. **Interactive Mode**: After tests, enter interactive mode for manual AT commands

## Test Results Interpretation

### GSM Signal Quality (CSQ)
- **0-9**: Poor signal
- **10-14**: Fair signal
- **15-19**: Good signal
- **20-31**: Excellent signal
- **99**: Unknown/not detectable

### GSM Network Registration Status
- **0**: Not registered, not searching
- **1**: Registered (home network)
- **2**: Not registered, searching
- **3**: Registration denied
- **5**: Registered (roaming)

### GPS Accuracy Indicators

#### HDOP (Horizontal Dilution of Precision)
- **< 1**: Ideal accuracy
- **1-2**: Excellent accuracy
- **2-5**: Good accuracy
- **5-10**: Moderate accuracy
- **10-20**: Fair accuracy
- **> 20**: Poor accuracy

#### Satellite Count
- **< 4**: Insufficient for 3D fix
- **4-5**: Adequate for basic positioning
- **6-7**: Good satellite coverage
- **8+**: Excellent satellite coverage

#### GPS Fix Types
- **No Fix**: No position calculation possible
- **2D Fix**: Latitude/longitude only (3-4 satellites)
- **3D Fix**: Lat/lon/altitude (4+ satellites)

### Expected Results
- **GSM Basic Tests**: Should all pass if hardware is connected properly
- **GSM Network Tests**: May fail if no SIM card or poor coverage
- **GSM SMS Tests**: Require registered SIM card
- **GSM GPRS Tests**: Need valid APN configuration
- **GSM HTTP Tests**: Require active GPRS connection
- **GPS Basic Tests**: Should pass if GPS module is powered and connected
- **GPS NMEA Tests**: Should pass if GPS is receiving satellite data
- **GPS Location Tests**: May take 5-15 minutes for first fix outdoors
- **GPS Performance Tests**: Require stable GPS fix for accurate results

## Troubleshooting Guide

### GSM Module Issues

#### All Tests Fail
- Check wiring connections (RX/TX pins)
- Verify power supply to GSM module (3.7-4.2V)
- Ensure SIM card is properly inserted
- Try different baud rates (9600, 38400, 115200)

#### Basic Tests Pass, Network Tests Fail
- Check SIM card PIN status
- Verify network coverage in your area
- Ensure SIM card is activated and has credit
- Check antenna connection

#### Network OK, GPRS Tests Fail
- Configure correct APN for your carrier
- Verify data plan is active
- Check account balance
- Some carriers require username/password

### GPS Module Issues

#### No GPS Data Received
- Check wiring connections (GPS RX/TX pins)
- Verify power supply to GPS module (3.3V or 5V)
- Ensure GPS antenna is connected
- Move to location with clear sky view
- GPS modules don't work well indoors

#### GPS Data Received but No Fix
- **Wait longer** - First fix can take 5-15 minutes
- **Move outdoors** - Buildings block satellite signals
- **Check satellite count** - Need 4+ for 3D fix
- **Clear obstructions** - Remove objects blocking sky view
- **Check HDOP** - Should be < 10 for reasonable accuracy

#### Poor GPS Accuracy
- **HDOP > 10** indicates poor accuracy
- **< 4 satellites** means insufficient coverage
- **Move to better location** with more open sky
- **Wait for more satellites** to be acquired
- **Avoid valleys, urban canyons** that block signals

#### GPS Fix Lost Frequently
- **Check antenna connection** - Loose connections cause dropouts
- **Verify power supply** - Voltage drops cause resets
- **Avoid interference** - Keep away from other electronics
- **Check for movement** - Rapid position changes affect tracking

### Common APN Settings
- **AT&T**: `AT+SAPBR=3,1,"APN","phone"`
- **Verizon**: `AT+SAPBR=3,1,"APN","vzwinternet"`
- **T-Mobile**: `AT+SAPBR=3,1,"APN","fast.t-mobile.com"`
- **Generic**: `AT+SAPBR=3,1,"APN","internet"`

## Configuration Options

### ModeConfig.h Additions
```cpp
#define AT_COMMAND_TESTING true       // Enable/disable AT testing
#define AT_TEST_TIMEOUT 60000         // Timeout for long tests
```

### Pin Configurations
Ensure correct pin assignments in `PinConfig.h`:
```cpp
#define GSM_RX_PIN 2  // Arduino pin connected to SIM800L TX
#define GSM_TX_PIN 3  // Arduino pin connected to SIM800L RX
```

## Advanced Usage

### Custom Test Sequences
You can create custom test sequences by calling individual test functions:
```cpp
gsm.runBasicATTests();    // Just basic tests
gsm.testATCommand("AT+COPS=?", "OK", "Operator Scan", 60000);
```

### Interactive AT Commands
Use the standalone tester's interactive mode to send any AT command:
```
AT+HTTPINIT
AT+HTTPPARA="URL","http://your-server.com"
AT+HTTPACTION=0
```

## Output Examples

### Successful Test Output
```
========================================
  BASIC COMMUNICATION AT COMMAND TESTS
========================================
[PASS] Basic AT Command
[PASS] Disable Echo
[PASS] Get Manufacturer - SIMCOM
[PASS] Get Model - SIM800L
[PASS] Get IMEI - IMEI: 123456789012345
[PASS] Signal Quality - RSSI: 15 dBm
```

### Failed Test Output
```
[FAIL] Network Registration Status
    Response: +CREG: 0,2
[FAIL] GPRS Attachment Status
    Response: +CGATT: 0
```

## Performance Notes

- **Full Test Suite**: Takes 3-5 minutes
- **Basic Tests**: 30 seconds
- **Network Scan**: Can take 30-60 seconds
- **Interactive Mode**: Real-time responses

## Integration with Existing Code

The AT command testing integrates seamlessly with the existing BikeTracker functionality:
- No interference with normal operation
- Only available in testing mode
- Uses existing GSM module instance
- Maintains all existing functionality

## Future Enhancements

Potential additions:
- Automated APN detection
- Signal strength monitoring
- Network quality assessment
- SMS testing with real phone numbers
- HTTP endpoint validation
- Performance benchmarking

## Support

For issues or questions:
1. Check troubleshooting guide above
2. Verify hardware connections
3. Test with standalone utility first
4. Check carrier-specific settings
5. Consult SIM800L datasheet for advanced AT commands