# Libraries & Dependencies Reference

## ESP-IDF Framework

| Property | Value |
|----------|-------|
| Version | ESP-IDF v5.5 |
| Target | ESP32 |
| Build System | CMake + Ninja |
| Path | E:\Dev\Espressif\frameworks\esp-idf-v5.5 |

---

## Project Components

### Local Components (in `components/` folder)

| Component | Description |
|-----------|-------------|
| **lvgl** | LVGL Graphics Library v7.x |
| **lvgl_esp32_drivers** | ESP32 display and touch drivers |
| **lv_examples** | LVGL demo applications |

### Component Structure

```
components/
├── lvgl/                      # LVGL core library
│   ├── lvgl/
│   │   ├── src/
│   │   │   ├── lv_core/       # Core functionality
│   │   │   ├── lv_draw/       # Drawing functions
│   │   │   ├── lv_font/       # Font rendering
│   │   │   ├── lv_hal/        # Hardware abstraction
│   │   │   ├── lv_misc/       # Utilities
│   │   │   ├── lv_themes/     # UI themes
│   │   │   └── lv_widgets/    # UI widgets
│   │   └── lvgl.h             # Main include file
│   └── lv_conf.h              # LVGL configuration
│
├── lvgl_esp32_drivers/        # Hardware drivers
│   ├── lvgl_helpers.c/h       # Driver initialization
│   ├── lvgl_spi_conf.h        # SPI configuration
│   ├── lvgl_tft/              # Display drivers
│   │   ├── disp_driver.c/h
│   │   ├── disp_spi.c/h
│   │   └── ili9341.c/h        # ILI9341 specific
│   └── lvgl_touch/            # Touch drivers
│       ├── touch_driver.c/h
│       ├── tp_spi.c/h
│       └── xpt2046.c/h        # XPT2046 specific
│
└── lv_examples/               # Demo applications
    └── lv_examples/
        └── src/
            └── lv_demo_widgets/
                └── lv_demo_widgets.c
```

---

## ESP-IDF Component Dependencies

### Main Application (main/CMakeLists.txt)

```cmake
idf_component_register(SRCS main.c
    INCLUDE_DIRS .
    REQUIRES 
        lvgl_esp32_drivers 
        lvgl_touch 
        lvgl_tft 
        lvgl 
        lv_examples 
        esp_event 
        esp_timer)
```

### LVGL ESP32 Drivers (lvgl_esp32_drivers/CMakeLists.txt)

```cmake
idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS .
    REQUIRES 
        lvgl 
        esp_driver_spi 
        esp_driver_gpio)
```

---

## Required ESP-IDF Components

| Component | Purpose | Header |
|-----------|---------|--------|
| freertos | Task management | freertos/FreeRTOS.h |
| esp_system | System functions | esp_system.h |
| esp_event | Event handling | esp_event.h |
| esp_timer | High-resolution timers | esp_timer.h |
| esp_driver_spi | SPI driver | driver/spi_master.h |
| esp_driver_gpio | GPIO driver | driver/gpio.h |
| log | Logging | esp_log.h |

---

## Adding New Dependencies

When including a new ESP-IDF header, add the component to CMakeLists.txt:

```cmake
# Example: Adding NVS Flash support
idf_component_register(SRCS main.c
    INCLUDE_DIRS .
    REQUIRES 
        lvgl_esp32_drivers 
        # ... existing requirements ...
        nvs_flash)          # Add new component
```

Common components and their headers:

| Header | Component to Add |
|--------|-----------------|
| nvs_flash.h | nvs_flash |
| esp_wifi.h | esp_wifi |
| esp_http_client.h | esp_http_client |
| esp_spiffs.h | spiffs |
| sdmmc_cmd.h | sdmmc |
| driver/i2c.h | esp_driver_i2c |
| driver/uart.h | esp_driver_uart |
| driver/ledc.h | esp_driver_ledc |

---

## LVGL Configuration (lv_conf.h)

Key configuration options:

```c
// Display settings
#define LV_HOR_RES_MAX          320
#define LV_VER_RES_MAX          240
#define LV_COLOR_DEPTH          16    // RGB565

// Memory settings
#define LV_MEM_CUSTOM           0     // Use LVGL's internal memory manager
#define LV_MEM_SIZE             (32U * 1024U)

// Feature enables
#define LV_USE_GPU              0     // No GPU on ESP32
#define LV_USE_FILESYSTEM       0     // File system support
#define LV_USE_ANIMATION        1     // Animations enabled

// Widget enables
#define LV_USE_ARC              1
#define LV_USE_BAR              1
#define LV_USE_BTN              1
#define LV_USE_BTNMATRIX        1
#define LV_USE_CALENDAR         1
#define LV_USE_CANVAS           1
#define LV_USE_CHART            1
#define LV_USE_CHECKBOX         1
#define LV_USE_CONT             1
#define LV_USE_DROPDOWN         1
#define LV_USE_GAUGE            1
#define LV_USE_IMG              1
#define LV_USE_KEYBOARD         1
#define LV_USE_LABEL            1
#define LV_USE_LED              1
#define LV_USE_LINEMETER        1
#define LV_USE_LIST             1
#define LV_USE_MSGBOX           1
#define LV_USE_PAGE             1
#define LV_USE_ROLLER           1
#define LV_USE_SLIDER           1
#define LV_USE_SWITCH           1
#define LV_USE_TABLE            1
#define LV_USE_TABVIEW          1
#define LV_USE_TEXTAREA         1

// Theme
#define LV_USE_THEME_MATERIAL   1
```

---

## Build System

### Build Commands

```bash
# Configure environment (PowerShell)
. E:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1

# Full build
idf.py build

# Clean build
idf.py fullclean
idf.py build

# Flash to device
idf.py -p COM3 flash

# Monitor output
idf.py -p COM3 monitor

# Combined flash and monitor
idf.py -p COM3 flash monitor

# Open menuconfig (interactive configuration)
idf.py menuconfig
```

### Build Output

```
build/
├── lvgl-demo.bin           # Application binary
├── lvgl-demo.elf           # ELF file for debugging
├── bootloader/
│   └── bootloader.bin      # Bootloader binary
├── partition_table/
│   └── partition-table.bin # Partition table
└── flash_args              # Flash command arguments
```

---

## Memory Usage

Typical memory allocation for this project:

| Memory Type | Usage | Available |
|-------------|-------|-----------|
| DRAM | ~180 KB | 320 KB |
| IRAM | ~50 KB | 128 KB |
| Flash | ~430 KB | 2 MB+ |

LVGL-specific memory:
- Display buffers: ~51 KB (2 × 320 × 40 × 2 bytes)
- LVGL heap: 32 KB (configurable)

---

## Kconfig Menu Structure

Access via `idf.py menuconfig`:

```
Component config
├── LVGL configuration
│   ├── Graphical settings
│   ├── Memory settings
│   ├── Input device settings
│   └── Feature enable/disable
├── LVGL TFT Display controller
│   ├── Display controller model
│   ├── Display orientation
│   ├── Display Pin Assignments
│   └── SPI settings
└── LVGL Touch controller
    ├── Touch controller type
    ├── Touch Pin Assignments
    └── Calibration values
```
