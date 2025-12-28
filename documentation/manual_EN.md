# Lindi Level - User Manual (English)

**Version:** DEV  
**© Syquens B.V. 2025**  
**Author:** V.N. Verbon

---

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Device Overview](#device-overview)
4. [Main Interface](#main-interface)
   - [Start Tab](#start-tab)
   - [Level Tab](#level-tab)
   - [Info Tab](#info-tab)
5. [Features](#features)
   - [Analog/Digital Clock](#analogdigital-clock)
   - [Spirit Level](#spirit-level)
   - [WiFi Connectivity](#wifi-connectivity)
   - [Settings](#settings)
6. [Troubleshooting](#troubleshooting)
7. [Technical Specifications](#technical-specifications)

---

## Introduction

Welcome to Lindi Level, a smart digital spirit level with integrated clock functionality. This device combines precision leveling measurements with time display, making it perfect for workshops, construction sites, and home improvement projects.

**Key Features:**
- High-precision dual-axis leveling (Pitch and Roll)
- WiFi-enabled with automatic time synchronization
- Switchable analog/digital clock display
- Customizable display themes (Light/Dark)
- Multi-language support (English/Dutch)
- Persistent settings storage

---

## Getting Started

### First Power-Up

1. **Power the device** using the USB-C port or external 5V supply
2. The device will boot and display the **Start** tab with clock
3. Wait 5-10 seconds for the system to initialize
4. If WiFi credentials are configured, the device will connect automatically

### Initial Configuration

Navigate to the **Info** tab to configure:
- **Timezone** - Set your local GMT offset
- **Winter Time** - Enable/disable daylight saving time compensation
- **Language** - Switch between English (EN) and Dutch (NL)

> **Note:** Language changes update most labels immediately. Tab names (Level, Info, etc.) require a restart - disconnect and reconnect the power connection.

---

## Device Overview

### Display

- **Screen:** 3.2" TFT LCD (320×240 pixels)
- **Touch:** Resistive touchscreen (XPT2046)
- **Backlight:** Always-on LED backlight

### Sensors

- **MPU6050:** 6-axis accelerometer/gyroscope for precise leveling
- **Measurement Range:** ±30° on both axes
- **Update Rate:** 10Hz (100ms refresh)

### Connectivity

- **WiFi:** 2.4GHz 802.11 b/g/n
- **NTP Sync:** Automatic time synchronization via internet

---

## Main Interface

The interface consists of three main tabs accessible via touch:

### Start Tab

**Purpose:** Primary time display

**Features:**
- **Analog Clock** - Classic round clock face with hour, minute, and second hands
- **Digital Clock** - Large digital time display (HH:MM:SS format)
- **Toggle Button** - Tap the clock to switch between analog and digital modes
- **Auto-save** - Your clock preference is saved automatically

**Usage:**
1. Tap anywhere on the clock display
2. The clock will toggle between analog and digital
3. Your preference is remembered after restart

---

### Level Tab

**Purpose:** Precision spirit level measurements

**Features:**
- **Pitch Indicator** - Vertical bar showing forward/backward tilt
- **Roll Indicator** - Horizontal bar showing left/right tilt
- **Numeric Readout** - Real-time degree measurements (±0.1°)
- **Visual Feedback** - Bar position indicates direction and magnitude

**How to Read:**

**Pitch (Forward/Backward Tilt):**
- Bar centered = Level (0°)
- Bar up = Tilted backward (positive degrees)
- Bar down = Tilted forward (negative degrees)

**Roll (Left/Right Tilt):**
- Bar centered = Level (0°)
- Bar right = Tilted to the right (positive degrees)
- Bar left = Tilted to the left (negative degrees)

**Usage Tips:**
1. Place device on the surface to measure
2. Wait 1-2 seconds for readings to stabilize
3. Adjust surface until both bars center at 0°
4. Readings update 10 times per second for real-time feedback

**Measurement Range:**
- Maximum: ±30° on each axis
- Precision: ±0.1°
- Recommended operating range: ±10° for best accuracy

---

### Info Tab

**Purpose:** Device settings and system information

**Sections:**

#### Version Information
- Firmware version with build date/time
- Copyright and author information

#### Network Status
- WiFi connection status (Connected/Disconnected)
- IP address when connected
- Automatically reconnects if connection drops

#### Settings

**Timezone:**
- Dropdown menu with GMT-12 to GMT+12
- Default: GMT+1 (Amsterdam)
- Affects clock display immediately

**Winter Time:**
- Toggle switch for daylight saving time
- ON = Subtract 1 hour from timezone
- OFF = Use standard time
- Saves automatically

**Show FPS/CPU:**
- Performance monitor toggle
- Shows frames per second and CPU usage
- Default: OFF (for cleaner display)

**Dark Theme:**
- Toggle between light and dark display themes
- Dark theme reduces eye strain in low light
- Saves automatically
- **Note:** Theme changes may require restart for full effect

**Invert Level:**
- Reverses pitch and roll measurements
- Useful if sensor is mounted backward
- Use if left tilt shows as right tilt
- Saves automatically

**Language:**
- Switch between English (EN) and Dutch (NL)
- OFF = English
- ON = Dutch
- **Important:** Restart required for language change

---

## Features

### Analog/Digital Clock

**Automatic Time Sync:**
- Connects to NTP servers (pool.ntp.org, time.google.com)
- Syncs time within seconds of WiFi connection
- Automatic reconnection if sync fails

**Clock Modes:**
- **Analog:** Traditional clock face, smooth second hand
- **Digital:** Large numbers, HH:MM:SS format, 24-hour display

**Timezone Handling:**
- Base timezone set in Info tab
- Winter Time option subtracts 1 hour
- Example: GMT+1 with Winter Time ON = GMT+0

---

### Spirit Level

**Technology:**
- MPU6050 6-axis MEMS sensor
- Accelerometer-based tilt measurement
- Temperature-compensated readings

**Calibration:**
- No user calibration required
- Factory calibrated for ±1° accuracy
- Self-stabilizing algorithm

**Best Practices:**
- Place on stable, flat reference surface
- Avoid vibration during measurement
- Allow 2-3 seconds for initial stabilization
- Keep sensor away from magnetic fields

---

### WiFi Connectivity

**Configuration:**
- WiFi credentials stored in device firmware
- Automatic connection on startup
- Up to 5 retry attempts
- Continues operation if WiFi unavailable

**Network Requirements:**
- 2.4GHz WiFi network
- WPA2 Personal encryption supported
- DHCP for automatic IP assignment
- Internet access for NTP sync (optional)

**Connection Indicators:**
- Info tab shows "Connected" or "Disconnected"
- IP address displayed when connected
- No visual indicator on other tabs

---

### Settings

**Persistent Storage:**
- All settings saved to non-volatile memory (NVS)
- Settings survive power loss and restarts
- No "Save" button required (auto-save)

**Saved Settings:**
- Clock mode (analog/digital)
- Timezone offset
- Winter time enabled/disabled
- Dark theme enabled/disabled
- Sensor inversion enabled/disabled
- Language preference (EN/NL)

**Language Switching:**
- Toggle the **EN/NL** switch in the Info tab
- Setting labels update immediately (Timezone, Winter Time, etc.)
- Tab names update after restart (disconnect and reconnect power)
- Your language preference is saved automatically

**Factory Reset:**
- Not available via user interface
- Contact support for reset procedures

---

## Troubleshooting

### Clock not updating
- **Check WiFi connection** in Info tab
- **Verify internet access** on your network
- **Wait 60 seconds** for initial NTP sync
- **Restart device** if time remains incorrect

### Level readings incorrect
- **Check "Invert Level" setting** - toggle if reversed
- **Place on known level surface** to verify
- **Restart device** to reset sensor
- **Avoid magnetic interference** (speakers, motors)

### WiFi won't connect
- **Verify network availability** (2.4GHz only)
- **Check WiFi credentials** (requires firmware update)
- **Move closer to router** for better signal
- **Wait for all 5 retry attempts** (30-60 seconds)

### Touch not responding
- **Tap firmly** on touchscreen
- **Clean screen** with soft, dry cloth
- **Restart device** if consistently unresponsive
- **Avoid screen protectors** that may reduce sensitivity

### Display too bright/dark
- **Use Dark Theme** for lower brightness appearance
- **No manual brightness control** in current version
- Brightness is hardware-controlled

### Language won't change
- **Restart required** after language toggle
- Toggle the Language switch in Info tab
- Power cycle the device
- Check that switch moved to desired position

---

## Technical Specifications

### Hardware

**Microcontroller:**
- ESP32-D0WD-V3 (Dual-core Xtensa LX6)
- Clock: 240 MHz
- Flash: 4MB
- RAM: 520KB SRAM

**Display:**
- Controller: ILI9341
- Size: 3.2" diagonal
- Resolution: 320×240 pixels
- Colors: 262K (18-bit)
- Interface: SPI

**Touch:**
- Controller: XPT2046
- Type: 4-wire resistive
- Interface: SPI
- Pressure-sensitive

**Sensor:**
- Model: MPU6050
- Interface: I2C (400kHz)
- Range: ±2g (accelerometer)
- Resolution: 16-bit
- Update rate: 100Hz (10ms internal)

**Connectivity:**
- WiFi: IEEE 802.11 b/g/n
- Frequency: 2.4GHz
- Security: WPA/WPA2
- Antenna: PCB trace antenna

### Software

**Framework:**
- ESP-IDF v5.5
- FreeRTOS
- LVGL v7 (GUI library)

**Features:**
- NVS flash storage
- SNTP time synchronization
- I2C device driver
- Multi-task architecture

**Performance:**
- Boot time: 3-5 seconds
- Display refresh: 20-25 FPS
- Level update rate: 10 Hz
- Clock update: 1 Hz

### Power

**Input:**
- Voltage: 5V DC
- Current: 200-400mA (typical)
- Connector: USB-C or GPIO header

**Consumption:**
- Idle: ~200mA
- Active (all features): ~400mA
- Deep sleep: Not implemented

---

## Support

For technical support, firmware updates, or feature requests:

**Contact:**
- Company: Syquens B.V.
- Author: V.N. Verbon
- Year: 2025

**Documentation:**
- Full technical documentation available in /documentation folder
- Source code: ESP-IDF based, custom firmware

---

**End of Manual**
