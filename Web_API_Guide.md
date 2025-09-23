# Web API Integration Guide

This document provides examples and guidelines for implementing a web API that can receive location data from the IoT Bike Tracker.

## API Endpoint Specification

### HTTP POST Request

**Endpoint**: Your API URL (configured in `APIConfig.h`)  
**Method**: `POST`  
**Content-Type**: `application/json`  
**Expected Response**: HTTP 200-299 for success

### Request Body Format

The bike tracker sends JSON data in the following format:

```json
{
    "deviceId": "BIKE_TRACKER_001",
    "latitude": 40.7128,
    "longitude": -74.0060,
    "timestamp": "1640995200000",
    "alertType": "MOTION_DETECTED",
    "signalStrength": 25
}
```

### Field Descriptions

| Field | Type | Description | Example |
|-------|------|-------------|---------|
| `deviceId` | String | Unique identifier for the tracker device | "BIKE_TRACKER_001" |
| `latitude` | Number | GPS latitude coordinate (decimal degrees) | 40.7128 |
| `longitude` | Number | GPS longitude coordinate (decimal degrees) | -74.0060 |
| `timestamp` | String | Unix timestamp in milliseconds | "1640995200000" |
| `alertType` | String | Type of alert (optional, empty for regular updates) | "MOTION_DETECTED" |
| `signalStrength` | Number | GSM signal strength (0-31, or -1 if unknown) | 25 |

### Alert Types

The `alertType` field can contain the following values:

- `""` (empty) - Regular location update
- `"MOTION_DETECTED"` - Unauthorized movement detected (GPS-based)
- `"SPEED_EXCEEDED"` - Speed limit exceeded (GPS-based)
- `"GEOFENCE_BREACH"` - Vehicle left safe area (GPS-based)
- `"SYSTEM_ERROR"` - System malfunction
- `"GPS_LOST"` - GPS signal lost for extended period
- `"GSM_LOST"` - GSM connection lost for extended period

**Note**: Battery monitoring and theft detection alerts require additional sensors not included in the basic GPS+GSM setup.

## Sample API Implementations

### Node.js/Express Example

```javascript
const express = require('express');
const app = express();

app.use(express.json());

app.post('/api/tracker', (req, res) => {
    const { deviceId, latitude, longitude, timestamp, alertType, signalStrength } = req.body;
    
    // Validate required fields
    if (!deviceId || !latitude || !longitude || !timestamp) {
        return res.status(400).json({ error: 'Missing required fields' });
    }
    
    // Log the received data
    console.log(`Received location from ${deviceId}:`);
    console.log(`  Location: ${latitude}, ${longitude}`);
    console.log(`  Timestamp: ${new Date(parseInt(timestamp))}`);
    console.log(`  Alert: ${alertType || 'None'}`);
    console.log(`  Signal: ${signalStrength}`);
    
    // Store in database (implement your storage logic here)
    storeLocationData({
        deviceId,
        latitude: parseFloat(latitude),
        longitude: parseFloat(longitude),
        timestamp: new Date(parseInt(timestamp)),
        alertType: alertType || null,
        signalStrength: parseInt(signalStrength)
    });
    
    // Send success response
    res.status(200).json({ 
        status: 'success', 
        message: 'Location data received',
        deviceId: deviceId
    });
});

function storeLocationData(data) {
    // Implement your database storage logic here
    // Examples: MongoDB, PostgreSQL, MySQL, etc.
}

app.listen(3000, () => {
    console.log('Bike Tracker API listening on port 3000');
});
```

### Python/Flask Example

```python
from flask import Flask, request, jsonify
from datetime import datetime
import json

app = Flask(__name__)

@app.route('/api/tracker', methods=['POST'])
def receive_location():
    try:
        data = request.get_json()
        
        # Validate required fields
        required_fields = ['deviceId', 'latitude', 'longitude', 'timestamp']
        for field in required_fields:
            if field not in data:
                return jsonify({'error': f'Missing field: {field}'}), 400
        
        # Extract data
        device_id = data['deviceId']
        latitude = float(data['latitude'])
        longitude = float(data['longitude'])
        timestamp = datetime.fromtimestamp(int(data['timestamp']) / 1000)
        alert_type = data.get('alertType', '')
        signal_strength = int(data.get('signalStrength', -1))
        
        # Log received data
        print(f"Received location from {device_id}:")
        print(f"  Location: {latitude}, {longitude}")
        print(f"  Timestamp: {timestamp}")
        print(f"  Alert: {alert_type or 'None'}")
        print(f"  Signal: {signal_strength}")
        
        # Store in database (implement your storage logic here)
        store_location_data({
            'device_id': device_id,
            'latitude': latitude,
            'longitude': longitude,
            'timestamp': timestamp,
            'alert_type': alert_type or None,
            'signal_strength': signal_strength
        })
        
        # Return success response
        return jsonify({
            'status': 'success',
            'message': 'Location data received',
            'deviceId': device_id
        }), 200
        
    except Exception as e:
        print(f"Error processing location data: {e}")
        return jsonify({'error': 'Internal server error'}), 500

def store_location_data(data):
    # Implement your database storage logic here
    # Examples: SQLAlchemy, PyMongo, etc.
    pass

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=3000, debug=True)
```

### PHP Example

```php
<?php
header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(['error' => 'Method not allowed']);
    exit;
}

$input = file_get_contents('php://input');
$data = json_decode($input, true);

// Validate required fields
$required_fields = ['deviceId', 'latitude', 'longitude', 'timestamp'];
foreach ($required_fields as $field) {
    if (!isset($data[$field])) {
        http_response_code(400);
        echo json_encode(['error' => "Missing field: $field"]);
        exit;
    }
}

// Extract data
$device_id = $data['deviceId'];
$latitude = floatval($data['latitude']);
$longitude = floatval($data['longitude']);
$timestamp = date('Y-m-d H:i:s', intval($data['timestamp']) / 1000);
$alert_type = $data['alertType'] ?? '';
$signal_strength = intval($data['signalStrength'] ?? -1);

// Log received data
error_log("Received location from $device_id:");
error_log("  Location: $latitude, $longitude");
error_log("  Timestamp: $timestamp");
error_log("  Alert: " . ($alert_type ?: 'None'));
error_log("  Signal: $signal_strength");

// Store in database (implement your storage logic here)
try {
    store_location_data([
        'device_id' => $device_id,
        'latitude' => $latitude,
        'longitude' => $longitude,
        'timestamp' => $timestamp,
        'alert_type' => $alert_type ?: null,
        'signal_strength' => $signal_strength
    ]);
    
    // Return success response
    echo json_encode([
        'status' => 'success',
        'message' => 'Location data received',
        'deviceId' => $device_id
    ]);
    
} catch (Exception $e) {
    error_log("Error storing location data: " . $e->getMessage());
    http_response_code(500);
    echo json_encode(['error' => 'Internal server error']);
}

function store_location_data($data) {
    // Implement your database storage logic here
    // Examples: PDO, MySQLi, etc.
}
?>
```

## Database Schema Examples

### MySQL/PostgreSQL Schema

```sql
CREATE TABLE bike_locations (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id VARCHAR(50) NOT NULL,
    latitude DECIMAL(10, 8) NOT NULL,
    longitude DECIMAL(11, 8) NOT NULL,
    timestamp TIMESTAMP NOT NULL,
    alert_type VARCHAR(50),
    signal_strength INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_timestamp (device_id, timestamp),
    INDEX idx_alert_type (alert_type)
);
```

### MongoDB Schema

```javascript
// Example document structure
{
    _id: ObjectId("..."),
    deviceId: "BIKE_TRACKER_001",
    latitude: 40.7128,
    longitude: -74.0060,
    timestamp: ISODate("2023-01-01T12:00:00.000Z"),
    alertType: "MOTION_DETECTED",
    signalStrength: 25,
    createdAt: ISODate("2023-01-01T12:00:05.000Z")
}

// Create indexes for better performance
db.bike_locations.createIndex({ "deviceId": 1, "timestamp": -1 });
db.bike_locations.createIndex({ "alertType": 1 });
```

## Testing Your API

You can test your API endpoint using curl:

```bash
curl -X POST https://your-api.com/api/tracker \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "BIKE_TRACKER_001",
    "latitude": 40.7128,
    "longitude": -74.0060,
    "timestamp": "1640995200000",
    "alertType": "MOTION_DETECTED",
    "signalStrength": 25
  }'
```

Expected response:
```json
{
    "status": "success",
    "message": "Location data received",
    "deviceId": "BIKE_TRACKER_001"
}
```

## Security Considerations

1. **HTTPS**: Always use HTTPS for encrypted communication
2. **API Keys**: Consider implementing API key authentication
3. **Rate Limiting**: Implement rate limiting to prevent abuse
4. **Input Validation**: Validate all input data thoroughly
5. **Error Handling**: Don't expose sensitive information in error messages

## Common Issues and Solutions

### Issue: Tracker can't connect to API
- **Check**: APN settings in `APIConfig.h`
- **Check**: Network coverage in the area
- **Check**: API endpoint URL is correct and accessible

### Issue: Data not being received
- **Check**: API endpoint is returning HTTP 200-299
- **Check**: Content-Type header is set to `application/json`
- **Check**: Server logs for error messages

### Issue: Invalid JSON data
- **Check**: API is properly parsing JSON request body
- **Check**: All required fields are present in the request
- **Check**: Data types match expected format

## Next Steps

1. Deploy your API to a cloud service (AWS, Google Cloud, Azure, etc.)
2. Configure HTTPS with a valid SSL certificate
3. Set up database storage for location history
4. Implement real-time alerts and notifications
5. Create a web dashboard for monitoring your bike tracker