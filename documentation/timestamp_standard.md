# Timestamp Standard for Lindi Sensor Data

## Overview
All sensor data published via MQTT uses a **standardized millisecond-precision Unix timestamp** to enable accurate correlation between different data sources (level sensors, GPS, etc.).

## Timestamp Format

### Standard Definition
- **Type**: `uint64_t` (unsigned 64-bit integer)
- **Unit**: Milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
- **Precision**: 1 millisecond
- **Range**: Valid until year 2262 (184,467,440,737,095 ms)

### Implementation Pattern

**CRITICAL**: All sensor logging tasks MUST use this exact pattern:

```c
// Get current time with millisecond precision
struct timeval tv;
gettimeofday(&tv, NULL);

// Calculate milliseconds since epoch
uint64_t timestamp_ms = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
```

**Required headers**:
```c
#include <sys/time.h>
#include <time.h>
```

### Why This Standard?

1. **Correlation Accuracy**: GPS and other sensors can be synchronized within millisecond precision
2. **No Timezone Confusion**: Always UTC, timezone conversion happens at analysis time
3. **Future-Proof**: 64-bit integer supports dates far into the future
4. **Cross-Platform**: Standard Unix timestamp works across all systems
5. **JSON Compatible**: Represented as number (though precision may be lost in JavaScript)

## Converting Timestamps

### To Human-Readable DateTime

**Python**:
```python
from datetime import datetime

# From MQTT message
timestamp_ms = 1735840123456

# Convert to datetime (UTC)
dt_utc = datetime.utcfromtimestamp(timestamp_ms / 1000.0)
print(dt_utc)  # 2025-01-02 12:15:23.456000

# Convert to datetime (with timezone)
from datetime import timezone
dt = datetime.fromtimestamp(timestamp_ms / 1000.0, tz=timezone.utc)
print(dt.isoformat())  # 2025-01-02T12:15:23.456000+00:00

# Apply local timezone (example: GMT+1)
import pytz
local_tz = pytz.timezone('Europe/Amsterdam')
dt_local = dt.astimezone(local_tz)
print(dt_local)  # 2025-01-02 13:15:23.456000+01:00
```

**JavaScript/Node.js**:
```javascript
const timestamp_ms = 1735840123456;

// Convert to Date object
const date = new Date(timestamp_ms);
console.log(date.toISOString());  // 2025-01-02T12:15:23.456Z

// Format for display
const options = {
  year: 'numeric',
  month: '2-digit',
  day: '2-digit',
  hour: '2-digit',
  minute: '2-digit',
  second: '2-digit',
  fractionalSecondDigits: 3,
  timeZone: 'UTC'
};
console.log(date.toLocaleString('en-US', options));
```

**C/C++ (ESP32)**:
```c
uint64_t timestamp_ms = 1735840123456;

// Convert to time_t (seconds)
time_t timestamp_sec = timestamp_ms / 1000;

// Get millisecond fraction
int milliseconds = timestamp_ms % 1000;

// Convert to struct tm
struct tm timeinfo;
gmtime_r(&timestamp_sec, &timeinfo);

// Format as string
char datetime_str[32];
strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
printf("%s.%03d UTC\n", datetime_str, milliseconds);
// Output: 2025-01-02 12:15:23.456 UTC
```

### From Human-Readable DateTime

**Python**:
```python
from datetime import datetime

# From string (UTC)
dt = datetime.strptime("2025-01-02 12:15:23.456", "%Y-%m-%d %H:%M:%S.%f")
timestamp_ms = int(dt.timestamp() * 1000)
print(timestamp_ms)  # 1735840123456
```

**JavaScript**:
```javascript
// From ISO string
const isoString = "2025-01-02T12:15:23.456Z";
const timestamp_ms = new Date(isoString).getTime();
console.log(timestamp_ms);  // 1735840123456
```

## Data Correlation Examples

### Correlating Level Data with GPS

**Python with pandas**:
```python
import pandas as pd

# Load MQTT level data
level_df = pd.read_json('level_data.json')
# Columns: timestamp_ms, client_id, pitch, roll

# Load GPS data (from external GPS logger)
gps_df = pd.read_csv('gps_data.csv')
# Columns: timestamp_ms, latitude, longitude, altitude, speed

# Merge on nearest timestamp (within 500ms tolerance)
merged = pd.merge_asof(
    level_df.sort_values('timestamp_ms'),
    gps_df.sort_values('timestamp_ms'),
    on='timestamp_ms',
    direction='nearest',
    tolerance=500  # 500ms max time difference
)

print(merged.head())
# Output:
#   timestamp_ms  client_id  pitch  roll  latitude  longitude  altitude  speed
# 0  1735840123456  lindi_AB  1.23  -0.45  52.0001   5.0001     12.5     45.2
# 1  1735840124456  lindi_AB  1.25  -0.43  52.0002   5.0002     12.6     45.3
```

### Time-Series Analysis

**Python**:
```python
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_json('level_data.json')

# Convert timestamp to datetime index
df['datetime'] = pd.to_datetime(df['timestamp_ms'], unit='ms')
df = df.set_index('datetime')

# Resample to 1-second averages
df_1s = df.resample('1S').mean()

# Plot pitch over time
plt.figure(figsize=(12, 6))
plt.plot(df_1s.index, df_1s['pitch'])
plt.xlabel('Time (UTC)')
plt.ylabel('Pitch (degrees)')
plt.title('Vehicle Pitch Over Time')
plt.grid(True)
plt.show()
```

### SQL Database Storage

**PostgreSQL**:
```sql
CREATE TABLE level_data (
    id SERIAL PRIMARY KEY,
    timestamp_ms BIGINT NOT NULL,
    timestamp TIMESTAMP GENERATED ALWAYS AS (
        to_timestamp(timestamp_ms / 1000.0)
    ) STORED,
    client_id VARCHAR(32),
    pitch FLOAT,
    roll FLOAT
);

CREATE INDEX idx_timestamp_ms ON level_data(timestamp_ms);
CREATE INDEX idx_timestamp ON level_data(timestamp);

-- Query with human-readable time
SELECT
    to_char(timestamp, 'YYYY-MM-DD HH24:MI:SS.MS') as datetime,
    pitch,
    roll
FROM level_data
WHERE timestamp_ms BETWEEN 1735840000000 AND 1735843600000
ORDER BY timestamp_ms;
```

## Implementation Checklist

When adding a new sensor that publishes to MQTT:

- [ ] Include `<sys/time.h>` and `<time.h>` headers
- [ ] Use `struct timeval` with `gettimeofday()`
- [ ] Calculate timestamp as: `(uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000)`
- [ ] Store as `uint64_t timestamp_ms` variable
- [ ] Add to JSON as `"timestamp_ms": <value>` (number, not string)
- [ ] Publish at consistent intervals (e.g., 1 Hz for level data)
- [ ] Document the sensor type, topic, and update rate

## Current Sensor Implementations

### Level Sensor (MPU6050)
- **Topic**: `lindi/device/level`
- **Update Rate**: 1 Hz (every 1000ms)
- **Implementation**: [main/main.c:1268-1325](../main/main.c#L1268-L1325)
- **Message Format**:
  ```json
  {
    "client_id": "lindi_AB12CD",
    "timestamp_ms": 1735840123456,
    "pitch": 1.234,
    "roll": -0.567
  }
  ```

### Future Sensors
Follow the same pattern for:
- GPS location data
- Temperature sensors
- Accelerometer data
- Battery voltage
- Any other time-series sensor data

## Common Pitfalls to Avoid

### ❌ WRONG: Using time(NULL)
```c
// DON'T DO THIS - only second precision
time_t now = time(NULL);
uint64_t timestamp_ms = now * 1000;  // Missing sub-second precision!
```

### ❌ WRONG: Using millis()
```c
// DON'T DO THIS - relative to boot time, not UTC
uint64_t timestamp_ms = millis();  // Not synchronized across devices!
```

### ❌ WRONG: Using local timezone
```c
// DON'T DO THIS - timezone confusion
struct tm local_time;
localtime_r(&now, &local_time);  // Use gmtime_r() instead!
```

### ✅ CORRECT: Standard implementation
```c
struct timeval tv;
gettimeofday(&tv, NULL);
uint64_t timestamp_ms = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
```

## NTP Synchronization

The ESP32 must have valid NTP sync for accurate timestamps:

1. **Initial sync** happens on WiFi connection (see `initialize_sntp()` in main.c:348)
2. **Periodic sync** occurs automatically (ESP-IDF SNTP library)
3. **Sync status** can be checked with `sntp_get_sync_status()`
4. **Before first sync**, timestamps will be incorrect (epoch start)

**Handling missing NTP sync**:
```c
// Check if time is valid before publishing
time_t now;
time(&now);
if (now < 1704067200) {  // 2024-01-01 00:00:00 UTC
    ESP_LOGW(TAG, "Time not synced yet, skipping sensor publish");
    return;
}

// Proceed with timestamp creation
struct timeval tv;
gettimeofday(&tv, NULL);
uint64_t timestamp_ms = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
```

## Testing Timestamps

### Verify Current Time
```bash
# On ESP32 serial console
I (12345) main: Current time: 2025-01-02 12:15:23 UTC
```

### Compare with System Time
```python
import time
import json
import paho.mqtt.client as mqtt

def on_message(client, userdata, msg):
    data = json.loads(msg.payload)
    device_timestamp_ms = data['timestamp_ms']
    system_timestamp_ms = int(time.time() * 1000)

    diff_ms = abs(device_timestamp_ms - system_timestamp_ms)

    print(f"Device time: {device_timestamp_ms}")
    print(f"System time: {system_timestamp_ms}")
    print(f"Difference:  {diff_ms} ms")

    if diff_ms > 1000:
        print("WARNING: Time difference > 1 second!")

client = mqtt.Client()
client.on_message = on_message
client.connect("mqtt.syquens.com", 8883)
client.subscribe("lindi/device/level")
client.loop_forever()
```

## Related Documentation
- [MQTT Level Logging](mqtt_level_logging.md) - MQTT message format
- [Serial Menu](serial_menu.md) - Timezone configuration
- [Storage](storage.md) - NVS settings

---

**Last Updated**: 2026-01-02
**Version**: 1.0
