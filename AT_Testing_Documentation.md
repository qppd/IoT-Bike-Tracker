# AT Command Testing Documentation

## Overview
This document describes the comprehensive AT command testing functionality added to the IoT Bike Tracker system. The testing suite allows you to verify GSM module functionality, troubleshoot connectivity issues, and validate SIM800L module behavior.

## Features Added

### 1. Integrated Testing in BikeTracker.ino
The main BikeTracker system now includes built-in AT command testing when running in `MODE_TESTING`. New commands have been added to the existing serial command interface.

### 2. Standalone Testing Utility
A separate `ATCommandTester.ino` file provides independent testing functionality that can be used without the full BikeTracker system.

## Available Test Commands

### Basic Commands (BikeTracker.ino - Testing Mode)
- `ATTEST` - Run all AT command tests (comprehensive suite)
- `ATBASIC` - Test basic AT commands (communication, info, signal)
- `ATNETWORK` - Test network registration and operator commands
- `ATSMS` - Test SMS functionality commands
- `ATGPRS` - Test GPRS/bearer connection commands
- `ATHTTP` - Test HTTP service commands

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

## Usage Instructions

### Using Integrated Testing (Recommended)

1. **Set Testing Mode**: Ensure `CURRENT_MODE` is set to `MODE_TESTING` in `ModeConfig.h`
2. **Upload and Connect**: Upload BikeTracker.ino and open Serial Monitor (9600 baud)
3. **Run Tests**: Type one of the AT test commands:
   ```
   ATTEST      // Run all tests (3-5 minutes)
   ATBASIC     // Quick basic tests (30 seconds)
   ATNETWORK   // Network tests (1-2 minutes)
   ```

### Using Standalone Tester

1. **Upload ATCommandTester.ino** to your Arduino
2. **Connect Serial Monitor** at 9600 baud
3. **Automatic Testing**: Tests run automatically on startup
4. **Interactive Mode**: After tests, enter interactive mode for manual AT commands

## Test Results Interpretation

### Signal Quality (CSQ)
- **0-9**: Poor signal
- **10-14**: Fair signal
- **15-19**: Good signal
- **20-31**: Excellent signal
- **99**: Unknown/not detectable

### Network Registration Status
- **0**: Not registered, not searching
- **1**: Registered (home network)
- **2**: Not registered, searching
- **3**: Registration denied
- **5**: Registered (roaming)

### Expected Results
- **Basic Tests**: Should all pass if hardware is connected properly
- **Network Tests**: May fail if no SIM card or poor coverage
- **SMS Tests**: Require registered SIM card
- **GPRS Tests**: Need valid APN configuration
- **HTTP Tests**: Require active GPRS connection

## Troubleshooting Guide

### All Tests Fail
- Check wiring connections (RX/TX pins)
- Verify power supply to GSM module (3.7-4.2V)
- Ensure SIM card is properly inserted
- Try different baud rates (9600, 38400, 115200)

### Basic Tests Pass, Network Tests Fail
- Check SIM card PIN status
- Verify network coverage in your area
- Ensure SIM card is activated and has credit
- Check antenna connection

### Network OK, GPRS Tests Fail
- Configure correct APN for your carrier
- Verify data plan is active
- Check account balance
- Some carriers require username/password

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