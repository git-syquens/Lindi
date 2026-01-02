# Lindi - ESP32 LVGL Smart Display

Lindi is an ESP32-based smart display project featuring a touchscreen UI built with LVGL v7. It includes an analog clock, system information display, and various interactive controls.

## Hardware

- **MCU**: ESP32-D0WD-V3 (Dual-core Xtensa LX6, 240 MHz)
- **Display**: ILI9341 TFT LCD (320×240, SPI)
- **Touch**: XPT2046 Resistive Touch Controller
- **Framework**: ESP-IDF v5.5 with LVGL v7.0

## Features

- **Analog/Digital Clock** - Switchable clock modes with real-time display and smooth second hand animation
- **Spirit Level** - Dual-axis precision leveling with MPU6050 sensor (Pitch & Roll)
- **Calibration System** - User calibration with offset storage for permanent mounting scenarios
- **WiFi Connectivity** - NTP time synchronization with automatic reconnection
- **Dark Theme Toggle** - Switchable UI theme with instant preview
- **Accent Color Picker** - 16-color customization for UI theme accent
- **Multi-Language Support** - English/Dutch language switching
- **Persistent Settings** - All preferences saved to NVS flash (timezone, theme, language, calibration, accent color)
- **Tabbed Interface** - Organized UI with Start, Level, and Info tabs

## Project Structure

```
Lindi/
├── main/
│   ├── main.c              # Main application code
│   └── CMakeLists.txt      # Build configuration
├── components/
│   ├── lvgl/               # LVGL graphics library v7.0
│   ├── lvgl_esp32_drivers/ # Display and touch drivers
│   └── lv_examples/        # LVGL examples
├── documentation/
│   ├── index.md            # Documentation index
│   ├── hardware.md         # GPIO pinout and hardware config
│   ├── lcd.md              # Display controller setup
│   ├── touch.md            # Touch controller configuration
│   ├── libraries.md        # Dependencies and components
│   ├── storage.md          # NVS and storage interfaces
│   ├── lvgl-elements.md    # LVGL UI element reference
│   ├── analog_clock.md     # Analog clock implementation details
│   ├── gyro_mpu6050.md     # MPU6050 sensor integration
│   ├── manual_EN.md        # Complete user manual (English)
│   ├── manual_NL.md        # Complete user manual (Dutch)
│   ├── quick_reference_EN.md # Quick reference card (English)
│   └── quick_reference_NL.md # Quick reference card (Dutch)
└── build/                  # Build output directory
```

## Getting Started

### Prerequisites

- ESP-IDF v5.5 installed
- ESP32 development board with ILI9341 display and XPT2046 touch

### Build and Flash

```powershell
# Set up ESP-IDF environment
. E:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1

# Build the project
idf.py build

# Flash to device
idf.py -p COM8 flash

# Monitor serial output
idf.py -p COM8 monitor
```

### Configuration

Use `idf.py menuconfig` to configure:
- WiFi credentials
- Display orientation
- Touch calibration
- Timezone settings

## Display Configuration

The ILI9341 is connected via SPI with the following default pins:

| Function | GPIO |
|----------|------|
| MOSI | 13 |
| MISO | 12 |
| CLK | 14 |
| CS | 15 |
| DC | 2 |
| RST | EN |
| Backlight | 21 |

## Touch Configuration

The XPT2046 touch controller shares the SPI bus:

| Function | GPIO |
|----------|------|
| CS | 33 |
| IRQ | 36 |

## Documentation

Detailed documentation is available in the `documentation/` folder:

- [Documentation Index](documentation/index.md)
- [Hardware Configuration](documentation/hardware.md)
- [Display Setup](documentation/lcd.md)
- [Touch Controller](documentation/touch.md)
- [LVGL Elements](documentation/lvgl-elements.md)
- [Analog Clock Implementation](documentation/analog_clock.md)

## Current Development Status

### Completed
- ✅ Basic UI framework with tabbed interface
- ✅ Digital clock with NTP sync and timezone support
- ✅ Analog clock with smooth animations and switchable modes
- ✅ Dark theme toggle with persistence
- ✅ Accent color picker (16 colors) with live preview
- ✅ Spirit level with dual-axis measurements (Pitch & Roll)
- ✅ MPU6050 sensor integration with 10Hz update rate
- ✅ User calibration system with offset storage
- ✅ Multi-language support (English/Dutch)
- ✅ Persistent settings (NVS storage for all preferences)
- ✅ WiFi auto-reconnection and NTP sync

### Planned
- 📋 WiFi credentials in NVS (runtime configurable via serial menu, no rebuild required for deployment)
- 📋 Additional sensor features
- 📋 Power management and sleep modes
- 📋 Advanced calibration wizard

## Based On

This project is based on [lv_port_esp32](https://github.com/lvgl/lv_port_esp32).

## Acknowledgments

- [LVGL](https://lvgl.io/) - Light and Versatile Graphics Library
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) - Espressif IoT Development Framework

