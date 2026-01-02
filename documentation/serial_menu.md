# Serial Settings Menu v2.0

## Overview
The serial menu provides comprehensive UART-based configuration for all device settings without requiring touchscreen interaction. Perfect for initial setup, debugging, remote configuration, and advanced settings management.

## Features
- ✅ Complete settings overview
- ✅ WiFi configuration instructions
- ✅ Time source selection (WiFi/NTP or manual NTP server)
- ✅ Primary and secondary MQTT server configuration
- ✅ Timezone, language, theme settings
- ✅ Manual level calibration with pitch/roll offsets
- ✅ Live sensor data display
- ✅ Factory reset with confirmation
- ✅ Device reboot
- ✅ Non-blocking operation (separate FreeRTOS task)

## Access

### Opening the Menu
1. Connect to device via USB serial (COM port)
2. Open serial monitor at **115200 baud, 8N1**
3. Press **'m'** key to open menu
4. Use number keys and letters to select options
5. Press **'q'** or **'x'** to exit menu

### Serial Monitor Recommendations
- **idf.py monitor**: `idf.py -p COM7 monitor` (exit with Ctrl+])
- **PuTTY**: 115200 baud, 8 data bits, no parity, 1 stop bit
- **Arduino IDE**: Serial Monitor at 115200 baud
- **Tera Term**: 115200 8N1

## Menu Structure

```
╔═══════════════════════════════════════════════════════╗
║          LINDI - Configuration Menu v2.0              ║
╚═══════════════════════════════════════════════════════╝

  GENERAL
  [1] View Current Settings
  [2] Configure WiFi (requires rebuild)
  [3] Set Timezone
  [4] Change Language (EN/NL)
  [5] Toggle Theme (Light/Dark)

  TIME SOURCE
  [6] Configure Time Source (WiFi/NTP)

  MQTT CONFIGURATION
  [7] Configure Primary MQTT Server
  [8] Configure Secondary MQTT Server

  LEVEL CALIBRATION
  [9] Configure Level Offsets
  [s] Show Live Sensor Data

  SYSTEM
  [f] Factory Reset
  [r] Reboot Device
  [q] Exit Menu
```

## Menu Options

### [1] View Current Settings
Displays complete configuration overview:
- **General**: Timezone, language, theme, clock mode
- **Time Source**: WiFi/NTP or manual NTP server
- **MQTT Primary**: Address, port, TLS, credentials
- **MQTT Secondary**: Failover server configuration
- **Level Offsets**: Pitch/roll calibration values

### [2] Configure WiFi
**Note**: WiFi credentials are compiled into firmware for security.

**To change WiFi**:
1. Edit `main/wifi_credentials.h`
2. Set `WIFI_SSID` and `WIFI_PASS`
3. Rebuild and reflash firmware

### [3] Set Timezone
Configure GMT offset for time display.
- **Range**: -12 to +12
- **Saved to**: NVS (`lindi_cfg:timezone`)
- **Requires**: Device restart

**Example**:
```
Enter new timezone [-12-12] (default: 1): -5
✓ Timezone set to GMT-5
  Restart device to apply changes.
```

### [4] Change Language
Switch between English and Nederlands.
- **[0]** English
- **[1]** Nederlands
- **Saved to**: NVS (`lindi_cfg:language`)
- **Requires**: Restart to update tab names

### [5] Toggle Theme
Switch between Light and Dark theme.
- **Saved to**: NVS (`lindi_cfg:dark_theme`)
- **Changes apply**: Immediately on GUI

### [6] Configure Time Source
Choose between automatic WiFi/NTP or manual NTP server.

**Options**:
1. **WiFi/Auto NTP** (default)
   - Uses pool.ntp.org and time.google.com
   - Automatic failover
2. **Manual NTP Server**
   - Custom NTP server IP or hostname
   - Useful for local/corporate NTP servers

**Example**:
```
Select time source:
  [1] WiFi/Auto NTP (pool.ntp.org, time.google.com)
  [2] Manual NTP Server IP

Choice [1-2] (default: 1): 2
Enter NTP server IP/hostname [default: pool.ntp.org]: 192.168.1.1
✓ NTP server set to: 192.168.1.1
  Restart device to apply changes.
```

### [7] Configure Primary MQTT Server
Configure main MQTT broker connection.

**Settings**:
- **Address**: Hostname or IP (e.g., mqtt.syquens.com)
- **Port**: 1-65535 (typical: 1883 for TCP, 8883 for TLS)
- **TLS**: Enable/disable TLS encryption
- **Username**: MQTT authentication user (optional)
- **Password**: MQTT authentication password (optional)

**Stored in**: NVS namespace `mqtt_cfg`
- `mqtt1_addr` (string)
- `mqtt1_port` (uint16)
- `mqtt1_tls` (uint8: 0=disabled, 1=enabled)
- `mqtt1_user` (string)
- `mqtt1_pass` (string)

**Example**:
```
Current settings:
  Address: mqtt.syquens.com
  Port:    8883
  TLS:     Enabled
  User:    23-09edh

Server address [default: mqtt.syquens.com]: broker.local
Port [1-65535] (default: 8883): 1883
Enable TLS (y/n) [default: y]: n
Username (leave empty for none) [default: 23-09edh]: myuser
Password: mypassword

✓ Primary MQTT server configured
  mqtt://broker.local:1883
  Restart device to apply changes.
```

### [8] Configure Secondary MQTT Server (Failover)
Optional backup MQTT broker for redundancy.

**Settings**: Same as primary server
- Address, port, TLS, username, password
- Leave address **empty** to disable secondary server

**Use case**: Failover when primary server unavailable

**Example (disable)**:
```
Configure secondary server? (leave address empty to disable)

Server address (empty to disable) [default: ]:
✓ Secondary MQTT server disabled
```

**Example (enable)**:
```
Server address (empty to disable) [default: ]: mqtt-backup.local
Port [1-65535] (default: 1883): 1883
Enable TLS (y/n) [default: n]: n
Username (leave empty for none) [default: ]:
✓ Secondary MQTT server configured
  mqtt://mqtt-backup.local:1883
  Restart device to apply changes.
```

### [9] Configure Level Offsets
Manual calibration and offset adjustment for spirit level.

**Options**:
1. **Auto-calibrate**: Place device on level surface, read current position as zero
2. **Manually set pitch offset**: Enter specific pitch correction (±30°)
3. **Manually set roll offset**: Enter specific roll correction (±30°)
4. **Reset offsets to zero**: Clear all calibration

**Offsets stored as**: int32 millidegrees in NVS
- `pitch_offset`: Pitch calibration (±30000 millidegrees)
- `roll_offset`: Roll calibration (±30000 millidegrees)

**Example (manual offset)**:
```
Current offsets:
  Pitch: +0.500°
  Roll:  -0.300°

Options:
  [1] Auto-calibrate (place level, read current position)
  [2] Manually set pitch offset
  [3] Manually set roll offset
  [4] Reset offsets to zero
  [0] Cancel

Choice [0-4] (default: 0): 2
Enter pitch offset (degrees) [-30.00-30.00] (default: 0.500): 1.250
✓ Pitch offset set to +1.250°
  Changes apply immediately.
```

**Example (auto-calibrate)**:
```
Choice [0-4] (default: 0): 1

⚠️  AUTO-CALIBRATION
1. Place device on PERFECTLY level surface
2. Wait 2 seconds for sensor to stabilize
3. Current reading will be saved as zero reference

Proceed with auto-calibration (y/n) [default: n]: y

Calibrating in 2 seconds...
Reading sensor...
✓ Calibration complete!
  New offsets: pitch=+0.823° roll=-0.412°
```

### [s] Show Live Sensor Data
Display real-time MPU6050 readings for 10 seconds.

**Shows**:
- Pitch (°)
- Roll (°)
- Temperature (°C)

Press any key to return to menu early.

**Example**:
```
  Pitch    Roll     Temp
  ------   ------   ------
  +1.23°   -0.45°   24.5°C
```

### [f] Factory Reset
**⚠️ DESTRUCTIVE OPERATION**

Erases all NVS settings and restores factory defaults.

**Settings erased**:
- Timezone, language, theme
- Time source configuration
- MQTT server settings (primary and secondary)
- Level calibration offsets
- All user preferences

**Settings preserved**:
- WiFi credentials (compiled in firmware)

**Confirmation required**: Type `RESET` exactly

**Example**:
```
⚠️  WARNING: This will erase ALL settings!

Settings that will be erased:
  • Timezone, language, theme preferences
  • Time source configuration
  • MQTT server settings
  • Level calibration offsets
  • All user settings

Settings that will be PRESERVED:
  • WiFi credentials (compiled in firmware)

Type 'RESET' to confirm: RESET

Erasing NVS...
✓ Factory reset complete!
  Rebooting in 3 seconds...
```

### [r] Reboot Device
Immediate safe restart.
- 2-second countdown
- No data loss (all settings committed to NVS)

## NVS Storage Structure

### Namespaces Used
| Namespace | Purpose |
|-----------|---------|
| `lindi_cfg` | General settings (timezone, language, theme, offsets) |
| `mqtt_cfg` | MQTT server configurations |
| `time_cfg` | Time source settings |

### Keys and Data Types

**lindi_cfg namespace**:
| Key | Type | Range | Default | Description |
|-----|------|-------|---------|-------------|
| `timezone` | int32 | -12 to +12 | 1 | GMT offset |
| `winter_time` | uint8 | 0 or 1 | 0 | DST enabled |
| `language` | uint8 | 0 or 1 | 0 | 0=EN, 1=NL |
| `dark_theme` | uint8 | 0 or 1 | 1 | Theme mode |
| `digital_mode` | uint8 | 0 or 1 | 0 | Clock display |
| `pitch_off` | int32 | ±30000 | 0 | Pitch cal (millideg) |
| `roll_off` | int32 | ±30000 | 0 | Roll cal (millideg) |
| `invert_level` | uint8 | 0 or 1 | 0 | Invert sensor |
| `accent_idx` | uint8 | 0-15 | 0 | Color index |

**mqtt_cfg namespace**:
| Key | Type | Max Length | Description |
|-----|------|------------|-------------|
| `mqtt1_addr` | string | 128 | Primary server address |
| `mqtt1_port` | uint16 | - | Primary server port |
| `mqtt1_tls` | uint8 | - | Primary TLS enable |
| `mqtt1_user` | string | 64 | Primary username |
| `mqtt1_pass` | string | 128 | Primary password |
| `mqtt2_addr` | string | 128 | Secondary server address |
| `mqtt2_port` | uint16 | - | Secondary server port |
| `mqtt2_tls` | uint8 | - | Secondary TLS enable |
| `mqtt2_user` | string | 64 | Secondary username |
| `mqtt2_pass` | string | 128 | Secondary password |

**time_cfg namespace**:
| Key | Type | Description |
|-----|------|-------------|
| `use_ntp` | uint8 | 1=WiFi/Auto, 0=Manual NTP |
| `ntp_server` | string | Manual NTP server address |

## Implementation Details

### Files
- **[main/serial_menu.h](../main/serial_menu.h)** - Public API
- **[main/serial_menu.c](../main/serial_menu.c)** - Implementation (~1040 lines)
- **[main/CMakeLists.txt](../main/CMakeLists.txt)** - Build config (includes serial_menu.c)

### Task Specifications
- **Task Name**: `serial_menu`
- **Stack Size**: 4096 bytes
- **Priority**: 5
- **Core Affinity**: None (any core)
- **UART**: UART0 (shared with console, 115200 baud)

### Input Handling
- **Character input**: Single character with timeout (10s default)
- **Line input**: Multi-character with backspace support
- **Integer input**: Range validation, default value support
- **Float input**: Range validation for calibration offsets
- **Boolean input**: y/n with default
- **String input**: Buffer size protection, default values

### Safety Features
- **Factory reset**: Requires typing "RESET" exactly
- **Range validation**: All numeric inputs validated
- **Default values**: Safe fallbacks for all settings
- **NVS commit**: Atomic write+commit operations
- **Error handling**: Graceful NVS operation failures

## Usage Examples

### Complete Session: Configure Everything
```bash
# Open serial monitor
idf.py -p COM7 monitor

# Press 'm' to open menu
m

# View current settings
1

# Configure timezone
3
Enter new timezone [-12-12] (default: 1): -5

# Configure primary MQTT
7
Server address [default: mqtt.syquens.com]: 192.168.1.100
Port [1-65535] (default: 8883): 1883
Enable TLS (y/n) [default: y]: n
Username (leave empty for none) [default: 23-09edh]:
✓ Primary MQTT server configured

# Configure secondary MQTT (failover)
8
Server address (empty to disable) [default: ]: mqtt-backup.local
Port [1-65535] (default: 1883): 1883
Enable TLS (y/n) [default: n]: n
✓ Secondary MQTT server configured

# Set manual level offsets
9
Options:
  [1] Auto-calibrate
  [2] Manually set pitch offset
  [3] Manually set roll offset
  [4] Reset offsets to zero
  [0] Cancel
Choice [0-4] (default: 0): 2
Enter pitch offset (degrees) [-30.00-30.00] (default: 0.000): 0.5
✓ Pitch offset set to +0.500°

# Reboot to apply
r
Rebooting device in 2 seconds...
```

## Troubleshooting

### Menu Doesn't Appear
**Problem**: Pressing 'm' has no effect

**Solutions**:
1. Verify baud rate: **115200**
2. Check UART connection (COM port)
3. Try uppercase 'M'
4. Restart device
5. Verify serial_menu task is running

### Settings Not Saving
**Problem**: Changes revert after reboot

**Solutions**:
1. Check NVS partition not corrupted
2. Verify sufficient NVS space (24KB default)
3. Look for NVS errors in logs
4. Try factory reset to clear corrupted NVS

### MQTT Not Connecting After Config
**Problem**: MQTT settings configured but not working

**Checks**:
1. **Did you restart?** Changes require reboot
2. Verify server address is reachable
3. Check port number (1883 for TCP, 8883 for TLS)
4. Verify TLS setting matches broker
5. Check username/password if required
6. View MQTT connection logs in serial monitor

### Level Offsets Not Applying
**Problem**: Manual offsets don't change readings

**Solutions**:
1. **No restart needed** - offsets apply immediately
2. Verify offset values are within ±30°
3. Check MPU6050 is working (option 's')
4. Try auto-calibration instead

### Factory Reset Fails
**Problem**: NVS erase error

**Solutions**:
1. Reflash firmware with `idf.py erase-flash flash`
2. Check NVS partition table in `partitions.csv`
3. Verify ESP32 flash is not damaged

## Future Enhancements
- [ ] WiFi scan and selection (no rebuild required)
- [ ] Network diagnostics (ping, DNS lookup)
- [ ] System stats viewer (heap, tasks, uptime)
- [ ] Backup/restore settings to SD card
- [ ] MQTT connection test from menu
- [ ] Batch configuration via file upload
- [ ] Remote menu access via Telnet

## Related Documentation
- [storage.md](storage.md) - NVS details
- [mqtt_integration.md](mqtt_integration.md) - MQTT configuration
- [manual_EN.md](manual_EN.md) - User manual
- [manual_NL.md](manual_NL.md) - Dutch user manual

---

**Last Updated**: 2026-01-02
**Version**: 2.0
