# MPU6050 Gyroscope & Accelerometer

## ⚠️ CRITICAL: Clone Sensor Detection

> **IMPORTANT**: This project uses an MPU6050 **CLONE** sensor that returns `WHO_AM_I = 0x70` instead of the standard `0x68`.
>
> **Your initialization code MUST accept BOTH values:**
> ```c
> uint8_t who_am_i;
> mpu6050_read_bytes(MPU6050_WHO_AM_I, &who_am_i, 1);
> 
> // Accept both 0x68 (original) and 0x70 (clone)
> if (who_am_i != 0x68 && who_am_i != 0x70) {
>     ESP_LOGE(TAG, "Invalid MPU6050 WHO_AM_I: 0x%02X", who_am_i);
>     return ESP_ERR_NOT_FOUND;
> }
> ```
>
> **Verified Configuration**:
> - ✅ I2C Address: `0x68` (standard)
> - ✅ WHO_AM_I Register: Returns `0x70` (clone variant)
> - ✅ GPIO: SDA=21, SCL=22
> - ✅ All sensor readings work perfectly (accel, gyro, temp)
>
> The sensor is fully functional - only the WHO_AM_I register differs from original.

## Overview
The MPU6050 is a 6-axis Motion Processing Unit that combines a 3-axis gyroscope and a 3-axis accelerometer on the same chip. It communicates via I2C and provides motion, orientation, and position sensing capabilities.

## Sensor Specifications

### MPU6050 Features
- **Gyroscope**: 3-axis angular rate sensor
  - Programmable range: ±250, ±500, ±1000, ±2000°/sec
  - 16-bit ADC resolution
- **Accelerometer**: 3-axis acceleration sensor
  - Programmable range: ±2g, ±4g, ±8g, ±16g
  - 16-bit ADC resolution
- **Built-in DMP** (Digital Motion Processor) for motion fusion algorithms
- **Temperature sensor**: -40°C to +85°C
- **I2C Interface**: Fast mode (400kHz) supported
- **Power**: 3.3V or 5V compatible (most modules have onboard regulator)

## Hardware Connection

### I2C Pinout (ESP32)

| MPU6050 Pin | ESP32 GPIO | Function | Notes |
|-------------|------------|----------|-------|
| VCC | 3.3V | Power supply | 3.3V or 5V depending on module |
| GND | GND | Ground | - |
| SCL | GPIO22 | I2C Clock (master) | Primary I2C interface |
| SDA | GPIO21 | I2C Data (master) | Primary I2C interface |
| XCL | - | Auxiliary I2C Clock | Leave unconnected if not using aux I2C |
| XDA | - | Auxiliary I2C Data | Leave unconnected if not using aux I2C |
| AD0 | GND or VCC | I2C address select | Default: GND for address 0x68 |
| INT | GPIO (optional) | Interrupt output | Optional for data ready/motion detect |

**Default I2C Address**:
- `0x68` when AD0 is connected to GND (default)
- `0x69` when AD0 is connected to VCC

**Auxiliary I2C (XCL/XDA)**:
- Used to connect additional I2C sensors (e.g., magnetometer HMC5883L for 9-axis)
- MPU6050 acts as I2C master to auxiliary devices
- Can pass through data from auxiliary sensors
- **Not needed for basic gyro/accel functionality** - leave unconnected

### Wiring Notes
- Pull-up resistors (4.7kΩ typical) required on SDA and SCL lines
- Most MPU6050 breakout boards have onboard pull-ups
- Keep I2C wires short (<30cm) for reliable communication

## I2C Configuration (ESP32)

### I2C Initialization Example
```c
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO    22        // GPIO for I2C clock
#define I2C_MASTER_SDA_IO    21        // GPIO for I2C data
#define I2C_MASTER_NUM       I2C_NUM_0 // I2C port number
#define I2C_MASTER_FREQ_HZ   100000    // I2C frequency (100kHz)
#define MPU6050_ADDR         0x68      // MPU6050 I2C address

esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) return err;
    
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}
```

## MPU6050 Registers

### Key Registers

| Register | Address | Description |
|----------|---------|-------------|
| WHO_AM_I | 0x75 | Device ID (0x68 original / **0x70 clone**) ⚠️ |
| PWR_MGMT_1 | 0x6B | Power management |
| GYRO_CONFIG | 0x1B | Gyroscope configuration |
| ACCEL_CONFIG | 0x1C | Accelerometer configuration |
| ACCEL_XOUT_H | 0x3B | Acceleration X-axis high byte |
| ACCEL_XOUT_L | 0x3C | Acceleration X-axis low byte |
| TEMP_OUT_H | 0x41 | Temperature high byte |
| TEMP_OUT_L | 0x42 | Temperature low byte |
| GYRO_XOUT_H | 0x43 | Gyroscope X-axis high byte |
| GYRO_XOUT_L | 0x44 | Gyroscope X-axis low byte |

## Basic Usage

### Initialization
```c
// Wake up MPU6050 (exit sleep mode)
uint8_t pwr_mgmt = 0x00;
i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, 
                            (uint8_t[]){0x6B, pwr_mgmt}, 2, 
                            1000 / portTICK_PERIOD_MS);

// Verify device ID
uint8_t who_am_i;
i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                              (uint8_t[]){0x75}, 1,
                              &who_am_i, 1,
                              1000 / portTICK_PERIOD_MS);

if (who_am_i == 0x68) {
    ESP_LOGI(TAG, "MPU6050 detected!");
} else {
    ESP_LOGE(TAG, "MPU6050 not found!");
}
```

### Reading Accelerometer Data
```c
uint8_t data[6];
int16_t accel_x, accel_y, accel_z;

// Read 6 bytes starting from ACCEL_XOUT_H
i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                              (uint8_t[]){0x3B}, 1,
                              data, 6,
                              1000 / portTICK_PERIOD_MS);

// Combine high and low bytes
accel_x = (int16_t)((data[0] << 8) | data[1]);
accel_y = (int16_t)((data[2] << 8) | data[3]);
accel_z = (int16_t)((data[4] << 8) | data[5]);

// Convert to g (assuming ±2g range, sensitivity = 16384 LSB/g)
float ax = accel_x / 16384.0f;
float ay = accel_y / 16384.0f;
float az = accel_z / 16384.0f;

ESP_LOGI(TAG, "Accel: X=%.2fg, Y=%.2fg, Z=%.2fg", ax, ay, az);
```

### Reading Gyroscope Data
```c
uint8_t data[6];
int16_t gyro_x, gyro_y, gyro_z;

// Read 6 bytes starting from GYRO_XOUT_H
i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                              (uint8_t[]){0x43}, 1,
                              data, 6,
                              1000 / portTICK_PERIOD_MS);

// Combine high and low bytes
gyro_x = (int16_t)((data[0] << 8) | data[1]);
gyro_y = (int16_t)((data[2] << 8) | data[3]);
gyro_z = (int16_t)((data[4] << 8) | data[5]);

// Convert to °/sec (assuming ±250°/sec range, sensitivity = 131 LSB/°/sec)
float gx = gyro_x / 131.0f;
float gy = gyro_y / 131.0f;
float gz = gyro_z / 131.0f;

ESP_LOGI(TAG, "Gyro: X=%.2f°/s, Y=%.2f°/s, Z=%.2f°/s", gx, gy, gz);
```

### Reading Temperature
```c
uint8_t data[2];
int16_t temp_raw;

// Read 2 bytes starting from TEMP_OUT_H
i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                              (uint8_t[]){0x41}, 1,
                              data, 2,
                              1000 / portTICK_PERIOD_MS);

// Combine high and low bytes
temp_raw = (int16_t)((data[0] << 8) | data[1]);

// Convert to Celsius: Temperature = (TEMP_OUT / 340) + 36.53
float temperature = (temp_raw / 340.0f) + 36.53f;

ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
```

## Sensitivity Scales

### Accelerometer Sensitivity (LSB/g)

| Range | Sensitivity | Register Value (ACCEL_CONFIG) |
|-------|-------------|-------------------------------|
| ±2g | 16384 | 0x00 |
| ±4g | 8192 | 0x08 |
| ±8g | 4096 | 0x10 |
| ±16g | 2048 | 0x18 |

### Gyroscope Sensitivity (LSB/°/sec)

| Range | Sensitivity | Register Value (GYRO_CONFIG) |
|-------|-------------|------------------------------|
| ±250°/s | 131 | 0x00 |
| ±500°/s | 65.5 | 0x08 |
| ±1000°/s | 32.8 | 0x10 |
| ±2000°/s | 16.4 | 0x18 |

## Common Applications

### Tilt Detection
```c
// Calculate pitch and roll from accelerometer
float pitch = atan2(ay, sqrt(ax*ax + az*az)) * 180.0 / M_PI;
float roll = atan2(-ax, az) * 180.0 / M_PI;

ESP_LOGI(TAG, "Pitch: %.1f°, Roll: %.1f°", pitch, roll);
```

### Motion Detection
```c
// Simple motion detection based on acceleration magnitude
float accel_magnitude = sqrt(ax*ax + ay*ay + az*az);
bool is_moving = (accel_magnitude > 1.2f || accel_magnitude < 0.8f);

if (is_moving) {
    ESP_LOGI(TAG, "Motion detected!");
}
```

## Calibration

### Offset Calibration Procedure
1. Place sensor on flat, stable surface
2. Read 100+ samples
3. Calculate average values
4. Store offsets and subtract from future readings

```c
// Example calibration for accelerometer Z-axis (should read ~1g when flat)
float az_offset = 0.0f;
const int samples = 100;

for (int i = 0; i < samples; i++) {
    // Read accel_z...
    az_offset += az;
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

az_offset = (az_offset / samples) - 1.0f;  // Should be 1g when flat
ESP_LOGI(TAG, "Z-axis offset: %.4f", az_offset);
```

## Troubleshooting

### Common Issues

| Problem | Possible Cause | Solution |
|---------|---------------|----------|
| Device not detected (WHO_AM_I ≠ 0x68) | Wrong I2C address | Check AD0 pin connection |
| | Wiring issue | Verify SDA/SCL connections |
| | No pull-up resistors | Add 4.7kΩ pull-ups |
| Erratic readings | Noise/EMI | Add capacitors, shorter wires |
| | No calibration | Perform offset calibration |
| Always reads zero | Sleep mode active | Wake up device (PWR_MGMT_1 = 0x00) |
| I2C timeout | Clock speed too high | Reduce to 100kHz |
| | Long wires | Keep I2C wires <30cm |

## Libraries & Resources

### Recommended ESP-IDF Libraries
- **esp-idf-lib**: Comprehensive component library with MPU6050 driver
  - GitHub: https://github.com/UncleRus/esp-idf-lib
  - Component: `mpu6050`

### Installation (esp-idf-lib)
```bash
cd components
git clone https://github.com/UncleRus/esp-idf-lib.git
```

Then in your component's `CMakeLists.txt`:
```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES esp-idf-lib
)
```

### Usage with esp-idf-lib
```c
#include <mpu6050.h>

mpu6050_dev_t dev = {0};

// Initialize
ESP_ERROR_CHECK(mpu6050_init_desc(&dev, MPU6050_ADDR, I2C_NUM_0, 
                                   GPIO_NUM_21, GPIO_NUM_22));
ESP_ERROR_CHECK(mpu6050_init(&dev));

// Read data
mpu6050_acceleration_t accel;
mpu6050_rotation_t gyro;
ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &gyro));

ESP_LOGI(TAG, "Accel: %.2f, %.2f, %.2f", accel.x, accel.y, accel.z);
ESP_LOGI(TAG, "Gyro: %.2f, %.2f, %.2f", gyro.x, gyro.y, gyro.z);
```

## Advanced Features

### Digital Motion Processor (DMP)
The MPU6050 has a built-in DMP for sensor fusion:
- Quaternion output
- Gesture detection
- Pedometer
- Tap detection

**Note**: DMP requires proprietary firmware from InvenSense. Consider using external fusion algorithms (e.g., Madgwick, Mahony) for open-source projects.

### Interrupt Configuration
```c
// Enable data ready interrupt
uint8_t int_enable = 0x01;  // DATA_RDY_EN
i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR,
                            (uint8_t[]){0x38, int_enable}, 2,
                            1000 / portTICK_PERIOD_MS);

// Configure interrupt pin (active high, push-pull, latched)
uint8_t int_pin_cfg = 0x20;  // LATCH_INT_EN
i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR,
                            (uint8_t[]){0x37, int_pin_cfg}, 2,
                            1000 / portTICK_PERIOD_MS);
```

## References

- **MPU6050 Datasheet**: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
- **Register Map**: MPU-6000/MPU-6050 Register Map and Descriptions Rev 4.2
- **ESP-IDF I2C Driver**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html

---

## Status

⏸️ **Not yet implemented** - This documentation is prepared for future integration

## Files
- [documentation/gyro_mpu6050.md](gyro_mpu6050.md) - This documentation
