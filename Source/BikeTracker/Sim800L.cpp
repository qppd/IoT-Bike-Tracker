// Sim800L.cpp
// Implementation for SIM800L GSM module

#include "Sim800L.h"

Sim800L::Sim800L(SoftwareSerial &serial) : gsmSerial(serial) {
    status = GSM_INIT;
    lastCommandTime = 0;
    lastDataActivity = 0;
    gprsConnected = false;
    currentAPN = "";
    currentUsername = "";
    currentPassword = "";
}

void Sim800L::begin(long baudrate) {
    gsmSerial.begin(baudrate);
    delay(2000);
}

bool Sim800L::initialize() {
    clearBuffer();
    
    // Test AT communication
    if (!sendATCommand("AT", "OK", 3000)) {
        status = GSM_ERROR;
        return false;
    }
    
    // Disable echo
    sendATCommand("ATE0", "OK", 3000);
    
    // Set SMS text mode
    if (!sendATCommand("AT+CMGF=1", "OK", 3000)) {
        status = GSM_ERROR;
        return false;
    }
    
    // Check network registration
    if (sendATCommand("AT+CREG?", "+CREG: 0,1", 10000) || 
        sendATCommand("AT+CREG?", "+CREG: 0,5", 10000)) {
        status = GSM_NETWORK_CONNECTED;
        return true;
    } else {
        status = GSM_NO_NETWORK;
        return false;
    }
}

GSMStatus Sim800L::getStatus() {
    return status;
}

bool Sim800L::sendATCommand(const String &command, const String &expectedResponse, int timeout) {
    clearBuffer();
    gsmSerial.println(command);
    lastCommandTime = millis();
    
    return waitForResponse(expectedResponse, timeout);
}

bool Sim800L::waitForResponse(const String &expected, int timeout) {
    String response = "";
    unsigned long startTime = millis();
    
    while (millis() - startTime < timeout) {
        while (gsmSerial.available()) {
            char c = gsmSerial.read();
            response += c;
            
            if (response.indexOf(expected) >= 0) {
                lastResponse = response;
                return true;
            }
            
            if (response.indexOf("ERROR") >= 0) {
                lastResponse = response;
                return false;
            }
        }
        delay(10);
    }
    
    lastResponse = response;
    return false;
}

void Sim800L::clearBuffer() {
    while (gsmSerial.available()) {
        gsmSerial.read();
    }
}

void Sim800L::sendSMS(const String &number, const String &message) {
    if (status != GSM_NETWORK_CONNECTED) {
        return;
    }
    
    clearBuffer();
    
    // Set SMS text mode
    gsmSerial.println("AT+CMGF=1");
    delay(1000);
    
    // Set recipient
    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(number);
    gsmSerial.println("\"");
    delay(1000);
    
    // Send message
    gsmSerial.print(message);
    gsmSerial.write(26); // Ctrl+Z to send
    delay(5000);
    
    // Clear any response
    clearBuffer();
}

bool Sim800L::sendLocationSMS(const String &number, const String &location, const String &alertType) {
    if (status != GSM_NETWORK_CONNECTED) {
        return false;
    }
    
    String message = "BikeTracker Alert";
    if (alertType.length() > 0) {
        message += " - " + alertType;
    }
    message += "\nLocation: " + location;
    message += "\nTime: " + String(millis() / 1000) + "s";
    
    sendSMS(number, message);
    return true;
}

bool Sim800L::available() {
    return gsmSerial.available();
}

String Sim800L::read() {
    if (gsmSerial.available()) {
        return gsmSerial.readStringUntil('\n');
    }
    return "";
}

bool Sim800L::isNetworkConnected() {
    return (status == GSM_NETWORK_CONNECTED);
}

int Sim800L::getSignalStrength() {
    if (sendATCommand("AT+CSQ", "+CSQ:", 3000)) {
        // Parse signal strength from response
        int start = lastResponse.indexOf("+CSQ: ") + 6;
        int end = lastResponse.indexOf(",", start);
        if (start > 5 && end > start) {
            return lastResponse.substring(start, end).toInt();
        }
    }
    return -1;
}

String Sim800L::getIMEI() {
    if (sendATCommand("AT+GSN", "OK", 3000)) {
        // Extract IMEI from response
        int start = lastResponse.indexOf('\n') + 1;
        int end = lastResponse.indexOf('\n', start);
        if (start > 0 && end > start) {
            return lastResponse.substring(start, end);
        }
    }
    return "Unknown";
}

void Sim800L::powerOn() {
    // Implementation depends on hardware setup
    // Usually involves toggling a power pin
}

void Sim800L::powerOff() {
    sendATCommand("AT+CPOWD=1", "OK", 5000);
}

bool Sim800L::initializeGPRS(const String &apn, const String &username, const String &password) {
    if (status != GSM_NETWORK_CONNECTED) {
        return false;
    }
    
    // Store APN credentials for reconnection
    currentAPN = apn;
    currentUsername = username;
    currentPassword = password;
    
    // First, close any existing GPRS connection
    sendATCommand("AT+SAPBR=0,1", "OK", 5000);
    delay(1000);
    
    // Configure bearer profile for GPRS
    if (!sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", 5000)) {
        return false;
    }
    
    // Set APN
    String apnCommand = "AT+SAPBR=3,1,\"APN\",\"" + apn + "\"";
    if (!sendATCommand(apnCommand, "OK", 5000)) {
        return false;
    }
    
    // Set username if provided
    if (username.length() > 0) {
        String userCommand = "AT+SAPBR=3,1,\"USER\",\"" + username + "\"";
        if (!sendATCommand(userCommand, "OK", 5000)) {
            return false;
        }
    }
    
    // Set password if provided
    if (password.length() > 0) {
        String passCommand = "AT+SAPBR=3,1,\"PWD\",\"" + password + "\"";
        if (!sendATCommand(passCommand, "OK", 5000)) {
            return false;
        }
    }
    
    // Enable automatic time synchronization
    sendATCommand("AT+CNTPCID=1", "OK", 3000);
    sendATCommand("AT+CNTP=\"pool.ntp.org\",0", "OK", 3000);
    sendATCommand("AT+CNTP", "OK", 30000);
    
    // Open GPRS connection with retries
    for (int attempts = 0; attempts < 3; attempts++) {
        if (sendATCommand("AT+SAPBR=1,1", "OK", 30000)) {
            delay(2000);
            
            // Verify GPRS connection
            if (sendATCommand("AT+SAPBR=2,1", "OK", 5000)) {
                if (lastResponse.indexOf("1,1,") >= 0) { // Check if status is connected
                    gprsConnected = true;
                    lastDataActivity = millis();
                    return true;
                }
            }
        }
        
        // Wait before retry
        delay(5000);
        
        // Close connection for retry
        sendATCommand("AT+SAPBR=0,1", "OK", 5000);
        delay(2000);
    }
    
    gprsConnected = false;
    return false;
}

bool Sim800L::sendHTTPPOST(const String &url, const String &jsonData, String &response) {
    return performHTTPRequest("POST", url, jsonData, response);
}

bool Sim800L::sendHTTPGET(const String &url, String &response) {
    return performHTTPRequest("GET", url, "", response);
}

bool Sim800L::sendLocationHTTP(const String &url, const String &deviceId, float latitude, float longitude, const String &alertType) {
    // Ensure GPRS connection is active
    if (!maintainConnection()) {
        return false;
    }
    
    // Create enhanced JSON payload with additional metadata
    String jsonData = "{";
    jsonData += "\"deviceId\":\"" + deviceId + "\",";
    jsonData += "\"latitude\":" + String(latitude, 6) + ",";
    jsonData += "\"longitude\":" + String(longitude, 6) + ",";
    jsonData += "\"timestamp\":\"" + String(millis()) + "\",";
    
    if (alertType.length() > 0) {
        jsonData += "\"alertType\":\"" + alertType + "\",";
    } else {
        jsonData += "\"alertType\":\"\",";
    }
    
    jsonData += "\"signalStrength\":" + String(getSignalStrength()) + ",";
    jsonData += "\"localIP\":\"" + getLocalIP() + "\",";
    jsonData += "\"imei\":\"" + getIMEI() + "\"";
    jsonData += "}";
    
    String response;
    
    // Retry logic for HTTP requests
    for (int attempt = 0; attempt < 3; attempt++) {
        if (sendHTTPPOST(url, jsonData, response)) {
            return true;
        }
        
        // If first attempt fails, try to reconnect GPRS
        if (attempt == 0) {
            if (!reconnectGPRS()) {
                continue; // Skip this attempt if reconnection fails
            }
        }
        
        // Wait before retry
        delay(2000 * (attempt + 1));
    }
    
    return false;
}

void Sim800L::disconnectGPRS() {
    // Terminate HTTP service first
    sendATCommand("AT+HTTPTERM", "OK", 5000);
    
    // Close GPRS connection
    sendATCommand("AT+SAPBR=0,1", "OK", 5000);
    gprsConnected = false;
}

// New enhanced methods implementation

bool Sim800L::isGPRSConnected() {
    if (!gprsConnected) {
        return false;
    }
    
    // Check GPRS bearer status
    if (sendATCommand("AT+SAPBR=2,1", "OK", 5000)) {
        return lastResponse.indexOf("1,1,") >= 0;
    }
    
    return false;
}

bool Sim800L::reconnectGPRS() {
    if (currentAPN.length() == 0) {
        return false; // No APN configured
    }
    
    // Disconnect first
    disconnectGPRS();
    delay(2000);
    
    // Reconnect with stored credentials
    return initializeGPRS(currentAPN, currentUsername, currentPassword);
}

bool Sim800L::checkInternetConnectivity() {
    if (!isGPRSConnected()) {
        return false;
    }
    
    // Try to ping a reliable server (Google DNS)
    if (sendATCommand("AT+CIPPING=\"8.8.8.8\"", "OK", 15000)) {
        // Wait for ping result
        delay(5000);
        return lastResponse.indexOf("+CIPPING: 1,") >= 0; // Success response
    }
    
    return false;
}

bool Sim800L::ensureGPRSConnection() {
    if (isGPRSConnected()) {
        return true;
    }
    
    // Try to reconnect
    return reconnectGPRS();
}

bool Sim800L::maintainConnection() {
    // Check if connection is still active
    if (!isGPRSConnected()) {
        return reconnectGPRS();
    }
    
    // Update activity timestamp
    lastDataActivity = millis();
    return true;
}

void Sim800L::resetConnection() {
    // Perform a complete reset of the connection
    disconnectGPRS();
    delay(3000);
    
    // Reset network registration
    sendATCommand("AT+CREG=0", "OK", 3000);
    delay(1000);
    sendATCommand("AT+CREG=1", "OK", 3000);
    delay(5000);
    
    // Re-initialize if network is available
    if (status == GSM_NETWORK_CONNECTED && currentAPN.length() > 0) {
        initializeGPRS(currentAPN, currentUsername, currentPassword);
    }
}

String Sim800L::getLocalIP() {
    if (sendATCommand("AT+SAPBR=2,1", "OK", 5000)) {
        // Extract IP from response: +SAPBR: 1,1,"10.x.x.x"
        int startQuote = lastResponse.indexOf("\"", lastResponse.indexOf("1,1,"));
        int endQuote = lastResponse.indexOf("\"", startQuote + 1);
        if (startQuote > 0 && endQuote > startQuote) {
            return lastResponse.substring(startQuote + 1, endQuote);
        }
    }
    return "0.0.0.0";
}

void Sim800L::enableAutoTimeSync() {
    // Enable automatic time synchronization from network
    sendATCommand("AT+CLTS=1", "OK", 3000);
    sendATCommand("AT+CALA=1", "OK", 3000);
}

bool Sim800L::setHTTPHeaders(const String &headers) {
    // Set custom HTTP headers if needed
    String headerCommand = "AT+HTTPPARA=\"USERDATA\",\"" + headers + "\"";
    return sendATCommand(headerCommand, "OK", 5000);
}

unsigned long Sim800L::getLastDataActivity() {
    return lastDataActivity;
}

int Sim800L::extractHTTPStatusCode(const String &response) {
    // Extract HTTP status code from +HTTPACTION response
    int actionStart = response.indexOf("+HTTPACTION:");
    if (actionStart >= 0) {
        int commaPos = response.indexOf(",", actionStart);
        if (commaPos > actionStart) {
            int secondComma = response.indexOf(",", commaPos + 1);
            if (secondComma > commaPos) {
                return response.substring(commaPos + 1, secondComma).toInt();
            }
        }
    }
    return -1;
}

bool Sim800L::performHTTPRequest(const String &method, const String &url, const String &data, String &response) {
    response = "";
    
    // Ensure GPRS connection is active
    if (!ensureGPRSConnection()) {
        return false;
    }
    
    // Terminate any existing HTTP session
    sendATCommand("AT+HTTPTERM", "OK", 2000);
    delay(500);
    
    // Initialize HTTP service
    if (!sendATCommand("AT+HTTPINIT", "OK", 5000)) {
        return false;
    }
    
    // Set HTTP parameters
    if (!sendATCommand("AT+HTTPPARA=\"CID\",1", "OK", 5000)) {
        sendATCommand("AT+HTTPTERM", "OK", 5000);
        return false;
    }
    
    // Set URL
    String urlCommand = "AT+HTTPPARA=\"URL\",\"" + url + "\"";
    if (!sendATCommand(urlCommand, "OK", 5000)) {
        sendATCommand("AT+HTTPTERM", "OK", 5000);
        return false;
    }
    
    // Set content type for POST requests
    if (method == "POST" && data.length() > 0) {
        if (!sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 5000)) {
            sendATCommand("AT+HTTPTERM", "OK", 5000);
            return false;
        }
        
        // Set request timeout
        sendATCommand("AT+HTTPPARA=\"REDIR\",1", "OK", 3000);
        sendATCommand("AT+HTTPPARA=\"TIMEOUT\",30", "OK", 3000);
        
        // Upload data
        String dataCommand = "AT+HTTPDATA=" + String(data.length()) + ",10000";
        gsmSerial.println(dataCommand);
        delay(1000);
        
        if (waitForResponse("DOWNLOAD", 5000)) {
            gsmSerial.print(data);
            if (!waitForResponse("OK", 10000)) {
                sendATCommand("AT+HTTPTERM", "OK", 5000);
                return false;
            }
        } else {
            sendATCommand("AT+HTTPTERM", "OK", 5000);
            return false;
        }
    }
    
    // Execute HTTP request
    String actionCommand = "AT+HTTPACTION=";
    if (method == "GET") {
        actionCommand += "0";
    } else if (method == "POST") {
        actionCommand += "1";
    } else {
        sendATCommand("AT+HTTPTERM", "OK", 5000);
        return false;
    }
    
    if (sendATCommand(actionCommand, "OK", 5000)) {
        // Wait for HTTP response with longer timeout
        if (waitForResponse("+HTTPACTION:", 30000)) {
            int statusCode = extractHTTPStatusCode(lastResponse);
            
            if (statusCode >= 200 && statusCode < 300) {
                // Read response data
                if (sendATCommand("AT+HTTPREAD", "OK", 10000)) {
                    response = lastResponse;
                    lastDataActivity = millis();
                }
                sendATCommand("AT+HTTPTERM", "OK", 5000);
                return true;
            } else {
                // Log error status
                response = "HTTP_ERROR_" + String(statusCode);
            }
        }
    }
    
    // Cleanup on error
    sendATCommand("AT+HTTPTERM", "OK", 5000);
    return false;
}
