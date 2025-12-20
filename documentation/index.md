# Lindi ESP32 LVGL Development Board Documentation

This documentation provides complete reference material for developing applications on this ESP32-based LVGL development board. All information is structured for easy consumption by developers and AI assistants.

## Board Overview

- **MCU**: ESP32 (Dual-core Xtensa LX6, 240 MHz)
- **Display**: ILI9341 TFT LCD (320x240, SPI)
- **Touch**: XPT2046 Resistive Touch Controller (SPI via GPIO bit-bang)
- **Framework**: ESP-IDF v5.5 with LVGL v7.x

## Documentation Index

| File | Description |
|------|-------------|
| [hardware.md](hardware.md) | Complete GPIO pinout, SPI bus configuration, and module pin mappings |
| [lcd.md](lcd.md) | ILI9341 display controller setup, SPI configuration, and driver initialization |
| [touch.md](touch.md) | XPT2046 touch controller configuration, calibration, and input handling |
| [libraries.md](libraries.md) | ESP-IDF components, LVGL configuration, and dependency management |
| [storage.md](storage.md) | SD card, SPI Flash, and NVS storage interfaces |
| [lvgl-elements.md](lvgl-elements.md) | Complete LVGL UI element reference with code examples |

## Quick Start

### Build Commands
```bash
# Set up ESP-IDF environment (first time per session)
. E:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1

# Build the project
idf.py build

# Flash to device
idf.py -p COMx flash

# Monitor serial output
idf.py -p COMx monitor
```

### Project Structure
```
Lindi/
├── main/
│   └── main.c              # Application entry point and GUI task
├── components/
│   ├── lvgl/               # LVGL graphics library
│   ├── lvgl_esp32_drivers/ # Display and touch drivers
│   └── lv_examples/        # Demo widgets and examples
├── sdkconfig               # Project configuration
└── documentation/          # This documentation
```

## Key Configuration Files

- **sdkconfig**: Contains all Kconfig options for display size, pins, touch calibration
- **components/lvgl_esp32_drivers/lvgl_spi_conf.h**: SPI bus and pin definitions
- **components/lvgl_esp32_drivers/lvgl_touch/xpt2046.h**: Touch controller pin definitions

## For AI Assistants

When working with this project:
1. Always source the ESP-IDF environment before running `idf.py` commands
2. Display uses HSPI (SPI2_HOST), Touch uses GPIO bit-bang (not hardware SPI)
3. LVGL task runs on Core 1 with a mutex for thread safety
4. Touch coordinates need calibration - see touch.md for details
5. Add required ESP-IDF components to CMakeLists.txt REQUIRES when including new headers
