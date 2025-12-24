# MPU6050 Sensor Advisory

## Important: WHO_AM_I Register Issue

### The Problem
This project uses an MPU6050 sensor that is a **clone/variant** which returns a different WHO_AM_I register value than the original.

- **Original MPU6050**: WHO_AM_I register (0x75) returns `0x68`
- **This sensor (clone)**: WHO_AM_I register (0x75) returns `0x70`

### I2C Communication
The sensor still communicates correctly on the I2C bus:
- **I2C Address**: `0x68` (standard)
- **SDA Pin**: 21
- **SCL Pin**: 22
- **Supply Voltage**: 3.3V

### Code Fix Required
When initializing the MPU6050, the WHO_AM_I check must accept **both** values:

```c
uint8_t who_am_i;
mpu6050_read_bytes(MPU6050_WHO_AM_I, &who_am_i, 1);

// Accept both 0x68 (original) and 0x70 (clone)
if (who_am_i != 0x68 && who_am_i != 0x70) {
    // Error: invalid sensor
}
```

### Previous Issues
If you encountered "MPU6050 not found" or similar errors in the past, this was likely due to the code only accepting `0x68` and rejecting the clone's `0x70` response. The sensor was always working - the validation was just too strict.

### Recommendation
Always check for both values in any MPU6050 initialization code to ensure compatibility with both original and clone sensors.

---
*Last updated: December 24, 2025*
