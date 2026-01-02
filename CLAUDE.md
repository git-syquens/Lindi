# Claude AI Assistant Guide for Lindi Project

**Project**: Lindi - ESP32 LVGL Smart Spirit Level & Clock
**Hardware**: ESP32-D0WD-V3, ILI9341 Display (320×240), XPT2046 Touch, MPU6050 Gyro/Accel
**Framework**: ESP-IDF v5.5, LVGL v7.0
**Location**: `e:\Dev\Lindi`

---

## 🚀 Quick Start Commands

### Build Environment Setup
```powershell
# Activate ESP-IDF v5.5 environment (required once per PowerShell session)
e:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1

# Navigate to project
cd e:\Dev\Lindi
```

### Build, Flash & Monitor

**⚠️ IMPORTANT**: ESP-IDF v5.5 does not work from Git Bash/MSYS (which Claude Code uses).

**From Claude Code / Git Bash**, use the helper script:
```bash
cmd.exe /c "e:\Dev\Lindi\build_and_flash.cmd"
```

**From PowerShell/CMD window** (normal manual use):
```powershell
# Full workflow (build + flash + monitor)
idf.py -p COM7 build flash monitor

# Individual commands
idf.py build                    # Build only
idf.py -p COM7 flash            # Flash only
idf.py -p COM7 monitor          # Monitor only
idf.py -p COM7 flash monitor    # Flash + Monitor
```

### Exit Monitor
Press `Ctrl+]` to exit the serial monitor.

---

## 📁 Project Structure

```
e:\Dev\Lindi\
├── main/
│   ├── main.c                  # Main application code
│   ├── clock_component.c/h     # Modular clock component
│   ├── wifi_credentials.h      # WiFi settings (NOT in git, use template)
│   └── CMakeLists.txt
├── components/
│   ├── lvgl/                   # LVGL v7 graphics library
│   ├── lvgl_esp32_drivers/     # Display & touch drivers
│   └── lv_examples/            # LVGL demo widgets
├── documentation/
│   ├── index.md                # Project overview
│   ├── hardware.md             # GPIO pinout & SPI config
│   ├── lcd.md                  # ILI9341 display setup
│   ├── touch.md                # XPT2046 touch controller
│   ├── lvgl-elements.md        # **LVGL UI reference (PRIMARY)**
│   ├── gyro_mpu6050.md         # MPU6050 sensor integration
│   ├── mpu6050_advisory.md     # **CRITICAL hardware notes**
│   ├── manual_EN.md            # User manual (English)
│   ├── manual_NL.md            # User manual (Dutch)
│   └── ai-prompt.md            # AI development guide
├── sessions/                   # Development session notes
├── build/                      # Build output (ignored by git)
├── sdkconfig                   # ESP-IDF configuration
├── partitions.csv              # Custom partition table (2MB app)
└── CLAUDE.md                   # This file
```

---

## 🔧 Hardware Configuration

### Display (ILI9341)
- **Resolution**: 320×240 pixels, landscape
- **Interface**: SPI (HSPI/SPI2_HOST)
- **Pins**: MOSI=13, MISO=12, CLK=14, CS=15, DC=2, RST=4

### Touch (XPT2046)
- **Type**: Resistive, 4-wire
- **Interface**: GPIO bit-bang (NOT hardware SPI)
- **Pins**: MISO=39, MOSI=32, CLK=25, CS=33, IRQ=36

### MPU6050 Sensor (⚠️ IMPORTANT)
- **Interface**: I2C @ 400kHz
- **Pins**: SDA=21, SCL=22
- **⚠️ CLONE SENSOR**: Returns `WHO_AM_I = 0x70` instead of `0x68`
- **⚠️ REQUIRES**: External 4.7kΩ pull-up resistors on SDA & SCL
- **Address**: 0x68 (AD0=GND)

### WiFi
- **Credentials**: Stored in `main/wifi_credentials.h` (NOT in git)
- **Template**: Copy `main/wifi_credentials.h.template` and edit

---

## 📚 Critical Documentation Files

### Before Writing Code, Read:
1. **[documentation/lvgl-elements.md](documentation/lvgl-elements.md)** - PRIMARY LVGL UI reference
2. **[documentation/mpu6050_advisory.md](documentation/mpu6050_advisory.md)** - Hardware requirements & clone sensor info
3. **[documentation/ai-prompt.md](documentation/ai-prompt.md)** - Development patterns & translation guide

### For Hardware Issues:
- [documentation/hardware.md](documentation/hardware.md) - Complete GPIO pinout
- [documentation/2.8寸-ESP32-WROOM-32 Specifications-EN.pdf](documentation/2.8寸-ESP32-WROOM-32 Specifications-EN.pdf) - Board specs
- [documentation/ESP32_CREEN.pdf](documentation/ESP32_CREEN.pdf) - Display module specs

### For Sensor Integration:
- [documentation/gyro_mpu6050.md](documentation/gyro_mpu6050.md) - MPU6050 complete reference
- [sessions/2025-12-25.md](sessions/2025-12-25.md) - I2C pull-up resistor fix

---

## 🎯 Key Features & Implementation

### 1. Analog/Digital Clock (Start Tab)
- **File**: `main/clock_component.c`
- **Toggle**: Tap clock to switch modes
- **Storage**: NVS persistence (`lindi_cfg:digital_mode`)
- **Fonts**: Montserrat 48 (digital), default (analog)
- **Details**: [documentation/analog_clock.md](documentation/analog_clock.md)

### 2. Spirit Level (Level Tab)
- **Sensor**: MPU6050 accelerometer
- **Measurements**: Pitch (forward/back), Roll (left/right)
- **Range**: ±30° (±0.1° precision)
- **Update Rate**: 10Hz (100ms)
- **Calibration**: User-adjustable zero reference
- **Spec**: [levelmenu/level_menu_spec.md](levelmenu/level_menu_spec.md)

### 3. Settings (Info Tab)
- **Language**: EN/NL (restart required for tab names)
- **Timezone**: GMT-12 to GMT+12
- **Winter Time**: DST toggle
- **Dark Theme**: Light/Dark mode
- **Accent Color**: 16-color palette picker with NVS persistence
- **Invert Level**: Reverse pitch/roll readings

---

## ⚙️ Build Configuration

### Flash Size & Partitions
- **Flash**: 4MB configured
- **Partition Table**: Custom (`partitions.csv`)
  - nvs: 24KB
  - phy_init: 4KB
  - factory: 2MB (app)

### LVGL Fonts Enabled
- Montserrat: 12, 14, 16, 48
- Located in: `components/lvgl/lvgl/lv_conf.h`

### I2C Configuration
```c
#define I2C_MASTER_SCL_IO    22
#define I2C_MASTER_SDA_IO    21
#define I2C_MASTER_FREQ_HZ   400000  // Requires 4.7kΩ external pull-ups
```

### WiFi Configuration
- File: `main/wifi_credentials.h` (create from template)
- Auto-connect on startup
- NTP sync: pool.ntp.org, time.google.com

---

## 🔍 Development Guidelines

### LVGL UI Development
1. **Always check** [documentation/lvgl-elements.md](documentation/lvgl-elements.md) first
2. **Reference demo patterns** in `components/lv_examples/lv_examples/src/lv_demo_widgets/`
3. **Thread safety**: All LVGL calls must use `xGuiSemaphore` mutex
4. **No full-screen redraws**: Update only changed elements

### Code Location Rules
- **UI code**: `main/main.c` or new files in `main/`
- **DO NOT modify**: `components/` (unless fixing driver bugs)
- **New components**: Add to `components/` with proper `CMakeLists.txt`

### Adding ESP-IDF Dependencies
When including new headers, update `main/CMakeLists.txt`:
```cmake
idf_component_register(SRCS main.c clock_component.c
    INCLUDE_DIRS .
    REQUIRES
        lvgl_esp32_drivers
        lvgl
        esp_wifi
        nvs_flash
        driver           # For I2C, GPIO, SPI
        # Add new components here
)
```

---

## 🐛 Known Issues & Solutions

### MPU6050 Not Detected
**Symptoms**: `ESP_ERR_TIMEOUT`, zero I2C devices found
**Solution**:
1. Add 4.7kΩ pull-up resistors on SDA and SCL to 3.3V
2. Connect AD0 pin to GND (forces address 0x68)
3. Code accepts both `WHO_AM_I = 0x68` and `0x70` (clone variant)
4. Details: [documentation/mpu6050_advisory.md](documentation/mpu6050_advisory.md)

### Display Flickering
**Cause**: Rapid LVGL invalidations during bar updates
**Status**: Under investigation (see [sessions/2025-12-24.md](sessions/2025-12-24.md))
**Current mitigation**: Change detection to prevent unnecessary updates

### Screen Brightness Dip
**Cause**: USB power budget (not code-related)
**Solution**: Use powered USB hub or direct high-power USB port
**Reference**: [sessions/2025-12-21.md](sessions/2025-12-21.md)

### Language Change Requires Restart
**Behavior**: Tab names only update after power cycle
**Reason**: Tabs created during initialization
**Workaround**: Documented in user manuals - expected behavior

---

## 🔒 Security & Git

### WiFi Credentials
- **Template**: `main/wifi_credentials.h.template`
- **Actual file**: `main/wifi_credentials.h` (in `.gitignore`)
- **Setup**: Copy template and edit with your credentials
- **Never commit**: The actual credentials file

### Git History Cleaned
- All WiFi passwords removed from git history (Dec 24, 2025)
- See [WIFI_SECURITY_CHANGES.md](WIFI_SECURITY_CHANGES.md) for details

---

## 📊 Performance Metrics

- **Boot Time**: 3-5 seconds
- **Display FPS**: 20-25 FPS
- **Level Update**: 10Hz (100ms refresh)
- **Clock Update**: 1Hz (1 second)
- **I2C Speed**: 400kHz (with external pull-ups)
- **Power**: 200-400mA @ 5V

---

## 🎓 Learning Resources

### LVGL v7 Documentation
- [LVGL v7 Docs](https://docs.lvgl.io/v7/)
- [Widget Examples](components/lv_examples/lv_examples/src/lv_demo_widgets/)

### ESP-IDF v5.5 Documentation
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/v5.5/)
- [I2C Driver API](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32/api-reference/peripherals/i2c.html)

### Project-Specific Guides
- [AI Development Prompt](documentation/ai-prompt.md) - Request translation guide
- [Session Notes](sessions/) - Development history and troubleshooting

---

## 🚨 Critical Rules for AI Assistants

### Auto-Build & Flash Policy

**⚠️ CRITICAL LIMITATION**: Claude Code cannot automatically build ESP-IDF projects because:
- Claude Code runs Bash commands from Git Bash/MSYS environment
- ESP-IDF v5.5 explicitly blocks MSYS/Mingw environments with error: "MSys/Mingw is not supported"
- Even PowerShell/CMD wrappers fail because parent shell detection persists

**Contrast with GitHub Copilot**:
- Copilot runs commands directly in VSCode's integrated terminal (PowerShell/CMD)
- This native terminal environment is fully compatible with ESP-IDF v5.5
- Copilot can successfully auto-build/flash using the same idf.py commands

**Manual Build Required**:
1. After code changes, user must manually build in native PowerShell/CMD terminal
2. Navigate to `e:\Dev\Lindi`
3. Run: `build_and_flash.cmd` or `idf.py -p COM7 build flash monitor`
4. This limitation affects ALL ESP-IDF projects in Claude Code, not just Lindi

### ESP-IDF Environment
- **Root**: `e:\Dev\Espressif`
- **Version**: ESP-IDF v5.5
- **Export**: `e:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1`
- **Never search** for ESP-IDF location - it's always at this path

### Code Quality
- **Verify working patterns**: Check demo code before inventing new patterns
- **Read before write**: Always read files before modifying
- **No over-engineering**: Keep solutions simple and focused
- **LVGL thread safety**: Always use `xGuiSemaphore` for LVGL calls

### MPU6050 Sensor Code
**MUST** accept both WHO_AM_I values:
```c
uint8_t who_am_i;
mpu6050_read_bytes(MPU6050_WHO_AM_I, &who_am_i, 1);

// Accept both 0x68 (original) and 0x70 (clone)
if (who_am_i != 0x68 && who_am_i != 0x70) {
    ESP_LOGE(TAG, "Invalid MPU6050 WHO_AM_I: 0x%02X", who_am_i);
    return ESP_ERR_NOT_FOUND;
}
```

---

## ⏱️ Timestamp Standard for Sensor Data

**CRITICAL**: All sensor data published via MQTT MUST use this standardized timestamp format for accurate cross-sensor correlation.

### Standard Implementation Pattern
```c
#include <sys/time.h>
#include <time.h>

// Get current time with millisecond precision
struct timeval tv;
gettimeofday(&tv, NULL);

// Calculate milliseconds since Unix epoch
uint64_t timestamp_ms = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
```

### Requirements
- **Type**: `uint64_t` (unsigned 64-bit integer)
- **Unit**: Milliseconds since Unix epoch (Jan 1, 1970 00:00:00 UTC)
- **Precision**: 1 millisecond
- **JSON field**: `"timestamp_ms"` (number, not string)
- **Always UTC**: Never use local time or timezone-adjusted timestamps

### Why This Standard?
1. **Correlation**: GPS and sensor data can be synchronized within millisecond precision
2. **No Timezone Confusion**: All timestamps in UTC, conversion happens during analysis
3. **Future-Proof**: 64-bit supports dates until year 2262
4. **Cross-Platform**: Standard Unix timestamp works everywhere
5. **Consistency**: Same format across all sensors (level, GPS, temperature, etc.)

### Converting to Human-Readable

**Python**:
```python
from datetime import datetime
timestamp_ms = 1735840123456
dt = datetime.utcfromtimestamp(timestamp_ms / 1000.0)
print(dt)  # 2025-01-02 12:15:23.456000
```

**C (ESP32)**:
```c
time_t sec = timestamp_ms / 1000;
int ms = timestamp_ms % 1000;
struct tm timeinfo;
gmtime_r(&sec, &timeinfo);
strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
printf("%s.%03d UTC\n", datetime_str, ms);
```

### Current Implementations
- **Level Sensor** (MPU6050): `lindi/device/level` @ 1 Hz
  - [main/main.c:1268-1325](main/main.c#L1268-L1325)

### For Complete Documentation
See [documentation/timestamp_standard.md](documentation/timestamp_standard.md) for:
- Detailed implementation guide
- Data correlation examples
- SQL database storage patterns
- Common pitfalls to avoid
- NTP synchronization requirements

---

## 📞 Support & Maintenance

**Author**: V.N. Verbon
**Company**: Syquens B.V.
**Year**: 2025

For detailed troubleshooting, see:
- [User Manual (EN)](documentation/manual_EN.md)
- [User Manual (NL)](documentation/manual_NL.md)
- [Session Notes](sessions/)

---

## ✅ Verification Checklist

Before considering a task complete:
- [ ] Code compiles without errors
- [ ] Flashed to device successfully (COM7)
- [ ] Serial monitor shows expected output
- [ ] No LVGL thread-safety violations
- [ ] NVS settings persist across reboots
- [ ] Updated relevant documentation if needed
- [ ] Tested on actual hardware (not just compile)

---

**Last Updated**: 2026-01-02
**Version**: 1.1
