# MPU6050 Sensor Advisory

## Hardware Requirements

### External Pull-up Resistors (REQUIRED)
The ESP32's internal pull-up resistors (~45kΩ) are **too weak** for reliable I2C communication at standard speeds. External pull-ups are mandatory.

**Required components:**
- **Two 4.7kΩ resistors** (1/4W or SMD 0805/1206)
  - One from **SDA (GPIO21)** to **3.3V**
  - One from **SCL (GPIO22)** to **3.3V**

**Location:** Resistors can be placed at either end of the I2C bus:
- At the ESP32 side (GPIO21/22 to 3.3V), OR
- At the MPU6050 module side (SDA/SCL pins to VCC)
- If your MPU6050 breakout board already has 10kΩ pull-ups, you can add 4.7kΩ in parallel to strengthen them

**Note:** Many cheap MPU6050 breakout boards have 10kΩ pull-ups pre-installed, which work at low speeds but may cause issues at 400kHz. Adding 4.7kΩ resistors improves reliability.

### Decoupling Capacitors (RECOMMENDED)
For stable operation, especially with longer wire runs:

- **100nF ceramic capacitor** across VCC and GND (close to MPU6050 module)
- **Optional: 10µF electrolytic** in parallel for additional supply stability

### Wire Connections
Keep I2C wires as short as possible (ideally < 10cm). Longer wires increase capacitance and require stronger pull-ups or lower speeds.

**Pinout:**
- **SDA**: GPIO21
- **SCL**: GPIO22  
- **VCC**: 3.3V (NOT 5V - will damage ESP32!)
- **GND**: Ground

## Software Configuration

### I2C Speed Settings
With proper 4.7kΩ external pull-ups, the I2C bus can run at:
- **400kHz** - Full speed, recommended with external pull-ups
- **100kHz** - Standard speed, very reliable
- **10kHz** - Only needed if using weak internal pull-ups (not recommended)

Current configuration in `main.c`:
```c
#define I2C_MASTER_FREQ_HZ   400000    // 400kHz with 4.7kΩ external pull-ups
```

### Internal Pull-ups Disabled
The code explicitly disables ESP32's internal pull-ups since external resistors are used:

```c
i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,  // Using external 4.7kΩ pull-ups
    .scl_pullup_en = GPIO_PULLUP_DISABLE,  // Using external 4.7kΩ pull-ups
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};
```

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

## Troubleshooting

### Sensor Not Detected
1. Check that 4.7kΩ pull-up resistors are installed on both SDA and SCL
2. Verify wiring: GPIO21=SDA, GPIO22=SCL, 3.3V (not 5V!), GND
3. Try lowering I2C speed to 100kHz temporarily
4. Use I2C scanner to verify device appears at 0x68 or 0x69

### Unstable Readings / Intermittent Errors
1. Add 100nF decoupling capacitor across VCC/GND at MPU6050
2. Shorten I2C wire length
3. Check for loose connections
4. Verify 3.3V supply voltage is stable

### Wrong Axis Values
The sensor orientation may not match the code assumptions. Check the calculated pitch/roll values and adjust axis mapping if needed.

---
*Last updated: December 25, 2025*
