# Storage Reference

## Storage Options

This ESP32 board supports multiple storage interfaces:

| Storage Type | Interface | GPIO Pins | Notes |
|--------------|-----------|-----------|-------|
| SD Card | VSPI | 5, 18, 19, 23 | External microSD |
| SPI Flash | VSPI | 21 (CS) | External flash |
| Internal Flash | - | - | For SPIFFS/LittleFS |
| NVS | - | - | Key-value storage |

---

## SD Card Interface

### Pin Configuration

| Signal | GPIO | Function |
|--------|------|----------|
| CS | 5 | Chip Select (active low) |
| CLK | 18 | SPI Clock |
| MISO | 19 | Master In Slave Out |
| MOSI | 23 | Master Out Slave In |

### SPI Bus: VSPI (SPI3_HOST)

---

## SD Card Implementation Example

### CMakeLists.txt Requirements

```cmake
idf_component_register(SRCS main.c
    REQUIRES 
        # ... existing ...
        fatfs 
        sdmmc 
        esp_driver_sdspi)
```

### Initialization Code

```c
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

#define SD_CS_PIN       5
#define SD_MISO_PIN     19
#define SD_MOSI_PIN     23
#define SD_CLK_PIN      18
#define MOUNT_POINT     "/sdcard"

esp_err_t init_sd_card(void)
{
    esp_err_t ret;
    
    // Options for mounting the filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    sdmmc_card_t *card;
    
    // SPI bus configuration
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI_PIN,
        .miso_io_num = SD_MISO_PIN,
        .sclk_io_num = SD_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    // Initialize SPI bus (if not already done)
    ret = spi_bus_initialize(VSPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        return ret;
    }
    
    // SD card SPI device configuration
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CS_PIN;
    slot_config.host_id = VSPI_HOST;
    
    // Mount filesystem
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = VSPI_HOST;
    
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, 
                                   &mount_config, &card);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Print card info
    sdmmc_card_print_info(stdout, card);
    
    return ESP_OK;
}
```

### File Operations

```c
// Write file
FILE *f = fopen("/sdcard/test.txt", "w");
if (f) {
    fprintf(f, "Hello from ESP32!\n");
    fclose(f);
}

// Read file
char buf[128];
f = fopen("/sdcard/test.txt", "r");
if (f) {
    fgets(buf, sizeof(buf), f);
    fclose(f);
    ESP_LOGI(TAG, "Read: %s", buf);
}
```

---

## NVS (Non-Volatile Storage)

NVS provides key-value storage in flash for configuration data.

### CMakeLists.txt Requirements

```cmake
idf_component_register(SRCS main.c
    REQUIRES 
        # ... existing ...
        nvs_flash)
```

### NVS Implementation

```c
#include "nvs_flash.h"
#include "nvs.h"

// Initialize NVS
esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Erase and retry
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

// Save a value
void nvs_save_int(const char *key, int32_t value)
{
    nvs_handle_t handle;
    nvs_open("storage", NVS_READWRITE, &handle);
    nvs_set_i32(handle, key, value);
    nvs_commit(handle);
    nvs_close(handle);
}

// Read a value
int32_t nvs_read_int(const char *key, int32_t default_value)
{
    nvs_handle_t handle;
    int32_t value = default_value;
    
    if (nvs_open("storage", NVS_READONLY, &handle) == ESP_OK) {
        nvs_get_i32(handle, key, &value);
        nvs_close(handle);
    }
    return value;
}

// Save string
void nvs_save_string(const char *key, const char *value)
{
    nvs_handle_t handle;
    nvs_open("storage", NVS_READWRITE, &handle);
    nvs_set_str(handle, key, value);
    nvs_commit(handle);
    nvs_close(handle);
}
```

---

## SPIFFS (SPI Flash File System)

SPIFFS provides a filesystem on the internal flash.

### CMakeLists.txt Requirements

```cmake
idf_component_register(SRCS main.c
    REQUIRES 
        # ... existing ...
        spiffs)
```

### SPIFFS Implementation

```c
#include "esp_spiffs.h"

esp_err_t init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS: %s", esp_err_to_name(ret));
        return ret;
    }
    
    size_t total = 0, used = 0;
    esp_spiffs_info(NULL, &total, &used);
    ESP_LOGI(TAG, "SPIFFS: %d/%d bytes used", used, total);
    
    return ESP_OK;
}
```

### Partition Table for SPIFFS

Add to `partitions.csv`:
```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x6000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000, 1M,
spiffs,   data, spiffs,  ,        512K,
```

---

## External SPI Flash

### Pin Configuration

| Signal | GPIO |
|--------|------|
| CS | 21 |
| (Shares VSPI bus) | 18, 19, 23 |

### Implementation Notes

- Share the VSPI bus with SD card
- Use different CS pins for each device
- Configure SPI device with appropriate clock speed (typically 20-40 MHz)

---

## Storage Selection Guide

| Use Case | Recommended Storage | Reason |
|----------|---------------------|--------|
| Configuration data | NVS | Fast, wear-leveled, key-value |
| Small files (<1MB) | SPIFFS | Simple, built-in flash |
| Large files | SD Card | Removable, large capacity |
| Firmware assets | SPIFFS or Flash partition | Embedded in firmware |
| User data | SD Card | Easy to backup/transfer |
| Calibration data | NVS | Persistent, small size |
