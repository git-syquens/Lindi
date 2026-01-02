# MQTT Level Data Logging

## Overview
Lindi automatically publishes pitch and roll level data to MQTT every second with millisecond-precision timestamps for correlation with external GPS data.

## Configuration

### MQTT Broker Settings
Configure MQTT broker via serial menu (press 'm' on serial console):
- **Primary MQTT server**: Menu option [7]
- **Secondary MQTT server** (failover): Menu option [8]

Default settings (see `main/mqtt_config.h`):
- **Broker**: mqtts://mqtt.syquens.com:8883
- **Base Topic**: `lindi`
- **TLS**: Enabled

### Client ID
- **Auto-generated**: `lindi_XXXXXX` (last 3 bytes of MAC address)
- **Custom**: Set `MQTT_CLIENT_ID` in `mqtt_config.h`

## Published Messages

### Topic Structure
```
lindi/device/level
```

### Message Format (JSON)
```json
{
  "client_id": "lindi_AB12CD",
  "timestamp_ms": 1704153600123,
  "pitch": 1.234,
  "roll": -0.567
}
```

### Field Descriptions
| Field | Type | Description |
|-------|------|-------------|
| `client_id` | string | MQTT client identifier (MAC-based or custom) |
| `timestamp_ms` | uint64 | Milliseconds since Unix epoch (Jan 1, 1970 00:00:00 UTC) |
| `pitch` | float | Forward/backward tilt in degrees (-30° to +30°) |
| `roll` | float | Left/right tilt in degrees (-30° to +30°) |

### Timestamp Precision
- **Resolution**: 1 millisecond
- **Source**: ESP32 system time (synchronized via NTP)
- **Format**: Unix timestamp in milliseconds
- **Example**: `1704153600123` = 2024-01-02 00:00:00.123 UTC

**Converting to datetime** (Python):
```python
from datetime import datetime
timestamp_ms = 1704153600123
dt = datetime.fromtimestamp(timestamp_ms / 1000.0)
print(dt)  # 2024-01-02 00:00:00.123000
```

### Publishing Rate
- **Interval**: 1 second (1 Hz)
- **QoS**: 0 (at most once delivery)
- **Retained**: No

## Implementation Details

### Task Configuration
- **Task Name**: `mqtt_sensor_log_task`
- **Stack Size**: 3072 bytes
- **Priority**: 5
- **Core Affinity**: Core 0 (same as MPU6050 sensor task)
- **Startup Delay**: 5 seconds (waits for MQTT connection)

### Thread Safety
- Uses `mpu_mutex` to safely read pitch/roll from sensor task
- Timeout: 100ms for mutex acquisition

### Logging
- Logs every 10th message to avoid console spam (every 10 seconds)
- Full logging available via MQTT broker subscription

## Usage Examples

### Subscribing to Level Data

**Using mosquitto_sub**:
```bash
mosquitto_sub -h mqtt.syquens.com -p 8883 \
  -t "lindi/device/level" \
  --capath /etc/ssl/certs \
  -u "23-09edh" -P "YOUR_PASSWORD"
```

**Using Python paho-mqtt**:
```python
import paho.mqtt.client as mqtt
import json

def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    print(f"Client: {data['client_id']}")
    print(f"Time: {data['timestamp_ms']}")
    print(f"Pitch: {data['pitch']:.3f}°")
    print(f"Roll: {data['roll']:.3f}°")
    print("---")

client = mqtt.Client()
client.username_pw_set("23-09edh", "YOUR_PASSWORD")
client.on_message = on_message
client.tls_set()
client.connect("mqtt.syquens.com", 8883)
client.subscribe("lindi/device/level")
client.loop_forever()
```

### Correlating with GPS Data

The millisecond-precision timestamp allows accurate correlation with GPS data:

```python
import pandas as pd

# Load MQTT sensor data
sensor_df = pd.DataFrame([
    {"timestamp_ms": 1704153600123, "pitch": 1.2, "roll": -0.5},
    {"timestamp_ms": 1704153601123, "pitch": 1.3, "roll": -0.4},
])

# Load GPS data (example)
gps_df = pd.DataFrame([
    {"timestamp_ms": 1704153600150, "lat": 52.0, "lon": 5.0},
    {"timestamp_ms": 1704153601100, "lat": 52.0001, "lon": 5.0001},
])

# Merge on nearest timestamp (tolerance: 500ms)
merged = pd.merge_asof(
    sensor_df.sort_values('timestamp_ms'),
    gps_df.sort_values('timestamp_ms'),
    on='timestamp_ms',
    direction='nearest',
    tolerance=500
)
print(merged)
```

## Troubleshooting

### No messages published
1. Check WiFi connection: Device must be connected
2. Check MQTT broker connectivity: Verify firewall/TLS settings
3. Check serial logs for MQTT connection errors
4. Verify MQTT credentials in serial menu or `mqtt_config.h`

### Incorrect timestamps
1. Ensure NTP sync is working (WiFi connected)
2. Check timezone setting in serial menu (option [3])
3. Timestamps are always in UTC (not affected by timezone setting)

### Messages too frequent/infrequent
- Modify `vTaskDelay(pdMS_TO_TICKS(1000))` in `mqtt_sensor_log_task()` (main.c:1323)
- Change `1000` to desired interval in milliseconds

## Related Documentation
- [serial_menu.md](serial_menu.md) - MQTT configuration via serial menu
- [mqtt_config.h](../main/mqtt_config.h) - Compile-time MQTT settings
- [storage.md](storage.md) - NVS storage for MQTT settings

---

**Last Updated**: 2026-01-02
**Version**: 1.0
