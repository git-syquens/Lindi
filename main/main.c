/* Lindi ESP32 Project

LCD Driver IC Support:
ILI9341 240*320 3.2"    14-25FPS
ILI9488 320*480 3.5"    3-7FPS

ENC28J60_INT	GPIO34
ENC28J60_CS		GPIO5
FLASH_CS		GPIO21
SD_CS			GPIO22

VSPI SPI3_HOST=2	SD, FLASH, ENC28J60
CS0		GPIO_5
SCLK	GPIO_18
MISO	GPIO_19
MOSI	GPIO_23
QUADWP	GPIO_22
QUADHD	GPIO_21

HSPI SPI2_HOST=1   LCD exclusive
CS0		GPIO_15
SCLK	GPIO_14
MISO	GPIO_12
MOSI	GPIO_13
QUADWP	GPIO_2
QUADHD	GPIO_4


LCD SPI Pin Definitions  HSPI    SPI2_HOST=1
Component config → LVGL TFT Display controller → Display Pin Assignments
MOSI (Master Out Slave In)			GPIO_13
MISO (Slave Out Master In)			None		GPIO_12
CLK (SCK / Serial Clock)			GPIO_14
CS (Slave Select)					GPIO_15
DC (Data / Command)					GPIO_2
Reset								GPIO_4
Backlight							None

Touch SPI Pin Definitions components\lvgl_esp32_drivers\lvgl_touch\xpt2046.h
Component config → LVGL Touch controller → Touchpanel (XPT2046) Pin Assignments
MISO (Master In Slave Out)			GPIO_39(SENSOR_VN input only)
MOSI (Master Out Slave In)			GPIO_32
CLK (SCK / Serial Clock)			GPIO_33
CS (Slave Select)					GPIO_25
IRQ (Interrupt Request)				GPIO_36(SENSOR_VP input only)      


==================================================Module Left==================================================
3V3			3V3			2		Power supply
Reset button	 EN			 3		High: chip enabled / Low: chip disabled / Note: Do not leave EN pin floating
TP_IRQ		SENSOR_VP	4		GPIO36, ADC1_CH0, RTC_GPIO0			Input only
TP_MISO		SENSOR_VN	5		GPIO39, ADC1_CH3, RTC_GPIO3			Input only
RLight		IO34		6		GPIO34, ADC1_CH6, RTC_GPIO4			Input only
IRRev		IO35		7		GPIO35, ADC1_CH7, RTC_GPIO5			Input only
TP_MOSI		IO32		8		GPIO32, XTAL_32K_P (32.768 kHz crystal input), ADC1_CH4, TOUCH9,RTC_GPIO9
TP_CS		IO33		9		GPIO33, XTAL_32K_N (32.768 kHz crystal output), ADC1_CH5, TOUCH8,RTC_GPIO8
TP_CLK		IO25		10		GPIO25, DAC_1, ADC2_CH8, RTC_GPIO6, EMAC_RXD0
DHT11		IO26		11		GPIO26, DAC_2, ADC2_CH9, RTC_GPIO7, EMAC_RXD1
TouchPad	IO27		12		GPIO27, ADC2_CH7, TOUCH7, RTC_GPIO17, EMAC_RX_DV
TFT_CLK		IO14		13		GPIO14, ADC2_CH6, TOUCH6, RTC_GPIO16, MTMS, HSPICLK,HS2_CLK, SD_CLK, EMAC_TXD2
TFT_MISO	IO12		14		GPIO12, ADC2_CH5, TOUCH5, RTC_GPIO15, MTDI, HSPIQ,HS2_DATA2, SD_DATA2, EMAC_TXD3

================================================Module Bottom==================================================
GND			GND			15		Ground
TFT_MOSI	IO13		16		GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER
XXXX		SD2			17		NC
XXXX		SD3			18		NC
XXXX		CMD			19		NC
XXXX		CLK			20		NC
XXXX		SD0			21		NC
XXXX		SD1			22		NC
TFT_CS		IO15		23		GPIO15, ADC2_CH3, TOUCH3, MTDO, HSPICS0, RTC_GPIO13,HS2_CMD, SD_CMD, EMAC_RXD3
TFT_DC		IO2			24		GPIO2, ADC2_CH2, TOUCH2, RTC_GPIO12, HSPIWP, HS2_DATA0,SD_DATA0

================================================Module Right==================================================
GND			GND			38		Ground
SD_MOSI		IO23		37		GPIO23, VSPID, HS1_STROBE
IIC_SDA		IO22		36		GPIO22, VSPIWP, U0RTS, EMAC_TXD1
UART_RXD	TXD0		35		GPIO1, U0TXD, CLK_OUT3, EMAC_RXD2
UART_TXD	RXD0		34		GPIO3, U0RXD, CLK_OUT2
IIC_SCL		IO21		33		GPIO21, VSPIHD, EMAC_TX_EN
XXX			NC			32		NC
SD_MISO		IO19		31		GPIO19, VSPIQ, U0CTS, EMAC_TXD0
SD_CLK		IO18		30		GPIO18, VSPICLK, HS1_DATA7
SD_CS		IO5			29		GPIO5, VSPICS0, HS1_DATA6, EMAC_RX_CLK
IRSend		IO17		28		GPIO17, HS1_DATA5, U2TXD, EMAC_CLK_OUT_180
RGBLight	IO16		27		GPIO16, HS1_DATA4, U2RXD, EMAC_CLK_OUT
TFT_Reset	IO4			26		GPIO4, ADC2_CH0, TOUCH0, RTC_GPIO10, HSPIHD, HS2_DATA1,SD_DATA1, EMAC_TX_ER
Flash button	IO0			25		GPIO0, ADC2_CH1, TOUCH1, RTC_GPIO11, CLK_OUT1,EMAC_TX_CLK

 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include <time.h>
#include <sys/time.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

// SD card and storage
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

// Littlevgl header files
#include "lvgl/lvgl.h"			// LVGL header file
#include "lvgl_helpers.h"		// Helper - hardware driver related
#include "clock_component.h"		// Modular clock component
#include "wifi_credentials.h"		// WiFi credentials (local only, not in git)

#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"

// WiFi Configuration
#define WIFI_MAXIMUM_RETRY  5

// WiFi event group bits
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "lindi";

// WiFi globals
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static bool wifi_connected = false;
static char wifi_ip_addr[16] = "Not connected";

// Time and timezone globals
static int timezone_offset = 1; // Default: Amsterdam (GMT+1)
static bool time_synced = false;
static lv_obj_t *timezone_selector = NULL;
static bool winter_time_enabled = false; // Default: off (use summer time)
static bool dark_theme_enabled = false;  // Default: light theme
static bool sensor_inverted = false;     // Default: sensor pins forward

// Language configuration
typedef enum {
    LANG_EN = 0,
    LANG_NL = 1
} language_t;

static language_t current_language = LANG_EN;  // Default: English

// Translation strings (index by language_t)
static const char *STR_TAB_START[] = {"Start", "Start"};
static const char *STR_TAB_LEVEL[] = {"Level", "Waterpas"};
static const char *STR_TAB_INFO[] = {"Info", "Info"};
static const char *STR_PITCH[] = {"Pitch", "Kanteling"};
static const char *STR_ROLL[] = {"Roll", "Helling"};
static const char *STR_TIMEZONE[] = {"Timezone:", "Tijdzone:"};
static const char *STR_WINTER_TIME[] = {"Winter Time", "Wintertijd"};
static const char *STR_SHOW_FPS[] = {"Show FPS/CPU", "Toon FPS/CPU"};
static const char *STR_DARK_THEME[] = {"Dark Theme", "Donker Thema"};
static const char *STR_INVERT_LEVEL[] = {"Invert Level", "Niveau omkeren"};
static const char *STR_LANGUAGE[] = {"EN/NL", "EN/NL"};
static const char *STR_CALIBRATE[] = {"Calibrate", "Kalibreer"};
static const char *STR_RESET[] = {"Reset", "Reset"};
static const char *STR_CONFIRM_CAL[] = {"Are you sure?", "Weet u het zeker?"};
static const char *STR_CAL_WARNING[] = {"Make sure RV is perfectly level!", "Zorg dat de camper perfect waterpas staat!"};
static const char *STR_YES[] = {"Yes", "Ja"};
static const char *STR_NO[] = {"No", "Nee"};

#define NVS_NAMESPACE "lindi_cfg"

// I2C Configuration
#define I2C_MASTER_SCL_IO    22        // GPIO for I2C clock
#define I2C_MASTER_SDA_IO    21        // GPIO for I2C data
#define I2C_MASTER_NUM       I2C_NUM_0 // I2C port number
#define I2C_MASTER_FREQ_HZ   400000    // I2C frequency (400kHz with 4.7kΩ external pull-ups)

// MPU6050 Configuration
#define MPU6050_ADDR         0x68      // MPU6050 I2C address (AD0=GND)
#define MPU6050_PWR_MGMT_1   0x6B      // Power management register
#define MPU6050_WHO_AM_I     0x75      // Device ID register
#define MPU6050_ACCEL_XOUT_H 0x3B      // Accelerometer X-axis high byte
#define MPU6050_GYRO_XOUT_H  0x43      // Gyroscope X-axis high byte
#define MPU6050_TEMP_OUT_H   0x41      // Temperature high byte

// SD Card Configuration (VSPI SPI3_HOST)
#define SD_CS_PIN            5         // GPIO for SD card CS
#define SD_MISO_PIN          19        // GPIO for SD card MISO
#define SD_MOSI_PIN          23        // GPIO for SD card MOSI
#define SD_CLK_PIN           18        // GPIO for SD card CLK
#define SD_MOUNT_POINT       "/sdcard" // Mount point for SD card

// Clock component handle
static clock_handle_t main_clock = NULL;

// MPU6050 sensor data
#define USE_FAKE_SENSOR_DATA 0  // Set to 1 for fake data, 0 for real MPU6050

static float current_pitch = 0.0f;
static float current_roll = 0.0f;
static float pitch_offset = 0.0f;  // Calibration offset for pitch
static float roll_offset = 0.0f;   // Calibration offset for roll
static SemaphoreHandle_t mpu_mutex = NULL;

// SD card global
static sdmmc_card_t *sd_card = NULL;

// Level menu UI objects
static lv_obj_t *pitch_bar = NULL;
static lv_obj_t *roll_bar = NULL;
static lv_obj_t *pitch_label = NULL;
static lv_obj_t *roll_label = NULL;

// Info tab UI label objects (for dynamic language updates)
static lv_obj_t *tz_label = NULL;
static lv_obj_t *winter_label = NULL;
static lv_obj_t *perf_label = NULL;
static lv_obj_t *theme_label = NULL;
static lv_obj_t *sensor_label = NULL;
static lv_obj_t *lang_label = NULL;

// Previous values for change detection (avoid unnecessary redraws)
static int16_t prev_pitch_mapped = 0;
static int16_t prev_roll_mapped = 0;

//LV_IMG_DECLARE(mouse_cursor_icon);			/*Declare the image file.*/


/*********************
 *      DEFINES
 *********************/
#define TAG "Lindi"
#define LV_TICK_PERIOD_MS 10

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
void guiTask(void *pvParameter);				// GUI任务
static void perf_monitor_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void clock_update_task(lv_task_t *task);
static void level_menu_update_task(lv_task_t *task);
static void timezone_selector_cb(lv_obj_t *dd, lv_event_t e);
static void winter_time_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void dark_theme_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void sensor_inversion_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void language_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void calibrate_confirm_cb(lv_obj_t *btn, lv_event_t e);
static void calibrate_cancel_cb(lv_obj_t *btn, lv_event_t e);
static void calibrate_btn_cb(lv_obj_t *btn, lv_event_t e);
static void reset_calibration_cb(lv_obj_t *btn, lv_event_t e);
static esp_err_t i2c_master_init(void);
static esp_err_t mpu6050_init(void);
static void mpu6050_read_task(void *pvParameters);

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi started, connecting...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retry connecting to WiFi (%d/%d)", s_retry_num, WIFI_MAXIMUM_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG, "Failed to connect to WiFi");
        }
        wifi_connected = false;
        strcpy(wifi_ip_addr, "Not connected");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        snprintf(wifi_ip_addr, sizeof(wifi_ip_addr), IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Got IP: %s", wifi_ip_addr);
        s_retry_num = 0;
        wifi_connected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// SNTP time sync notification callback
void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synchronized via NTP");
    time_synced = true;
}

// Initialize SNTP for time synchronization
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "time.google.com");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
}

// Load winter time setting from NVS
void load_winter_time_setting(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t value = 0;
        err = nvs_get_u8(nvs_handle, "winter_time", &value);
        if (err == ESP_OK) {
            winter_time_enabled = (value != 0);
            ESP_LOGI(TAG, "Loaded winter time setting: %s", winter_time_enabled ? "enabled" : "disabled");
        } else {
            ESP_LOGI(TAG, "Winter time setting not found, using default (disabled)");
        }
        nvs_close(nvs_handle);
    }
}

// Save winter time setting to NVS
void save_winter_time_setting(bool enabled)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_u8(nvs_handle, "winter_time", enabled ? 1 : 0);
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved winter time setting: %s", enabled ? "enabled" : "disabled");
            }
        }
        nvs_close(nvs_handle);
    }
}

// Load dark theme setting from NVS
void load_dark_theme_setting(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t value = 0;
        err = nvs_get_u8(nvs_handle, "dark_theme", &value);
        if (err == ESP_OK) {
            dark_theme_enabled = (value != 0);
            ESP_LOGI(TAG, "Loaded dark theme setting: %s", dark_theme_enabled ? "enabled" : "disabled");
        } else {
            ESP_LOGI(TAG, "Dark theme setting not found, using default (disabled)");
        }
        nvs_close(nvs_handle);
    }
}

// Save dark theme setting to NVS
void save_dark_theme_setting(bool enabled)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_u8(nvs_handle, "dark_theme", enabled ? 1 : 0);
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved dark theme setting: %s", enabled ? "enabled" : "disabled");
            }
        }
        nvs_close(nvs_handle);
    }
}

// Load sensor inversion setting from NVS
void load_sensor_inversion_setting(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t value = 0;
        err = nvs_get_u8(nvs_handle, "sensor_inv", &value);
        if (err == ESP_OK) {
            sensor_inverted = (value != 0);
            ESP_LOGI(TAG, "Loaded sensor inversion setting: %s", sensor_inverted ? "inverted" : "normal");
        }
        nvs_close(nvs_handle);
    }
}

// Save sensor inversion setting to NVS
void save_sensor_inversion_setting(bool inverted)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_u8(nvs_handle, "sensor_inv", inverted ? 1 : 0);
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved sensor inversion setting: %s", inverted ? "inverted" : "normal");
            }
        }
        nvs_close(nvs_handle);
    }
}

// Load language setting from NVS
void load_language_setting(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t value = 0;
        err = nvs_get_u8(nvs_handle, "language", &value);
        if (err == ESP_OK) {
            current_language = (value == 1) ? LANG_NL : LANG_EN;
            ESP_LOGI(TAG, "Loaded language setting: %s", current_language == LANG_NL ? "NL" : "EN");
        }
        nvs_close(nvs_handle);
    }
}

// Save language setting to NVS
void save_language_setting(language_t lang)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_u8(nvs_handle, "language", lang == LANG_NL ? 1 : 0);
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved language setting: %s", lang == LANG_NL ? "NL" : "EN");
            }
        }
        nvs_close(nvs_handle);
    }
}

// Load calibration offsets from NVS
void load_calibration_offsets(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        int32_t pitch_val = 0, roll_val = 0;
        // Load pitch offset (stored as int32 * 1000 for precision)
        err = nvs_get_i32(nvs_handle, "pitch_off", &pitch_val);
        if (err == ESP_OK) {
            pitch_offset = pitch_val / 1000.0f;
            ESP_LOGI(TAG, "Loaded pitch offset: %.3f°", pitch_offset);
        }
        // Load roll offset
        err = nvs_get_i32(nvs_handle, "roll_off", &roll_val);
        if (err == ESP_OK) {
            roll_offset = roll_val / 1000.0f;
            ESP_LOGI(TAG, "Loaded roll offset: %.3f°", roll_offset);
        }
        nvs_close(nvs_handle);
    }
}

// Save calibration offsets to NVS
void save_calibration_offsets(float pitch_off, float roll_off)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        // Store as int32 * 1000 for precision
        int32_t pitch_val = (int32_t)(pitch_off * 1000.0f);
        int32_t roll_val = (int32_t)(roll_off * 1000.0f);
        
        err = nvs_set_i32(nvs_handle, "pitch_off", pitch_val);
        if (err == ESP_OK) {
            err = nvs_set_i32(nvs_handle, "roll_off", roll_val);
        }
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved calibration offsets: pitch=%.3f° roll=%.3f°", pitch_off, roll_off);
            }
        }
        nvs_close(nvs_handle);
    }
}

// Reset calibration offsets
void reset_calibration_offsets(void)
{
    pitch_offset = 0.0f;
    roll_offset = 0.0f;
    save_calibration_offsets(0.0f, 0.0f);
    ESP_LOGI(TAG, "Calibration offsets reset to zero");
}

// Initialize SD card on VSPI bus
static esp_err_t init_sd_card(void)
{
    esp_err_t ret;
    
    ESP_LOGI(TAG, "Initializing SD card on VSPI...");
    
    // Options for mounting the filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    // SPI bus configuration for VSPI
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI_PIN,
        .miso_io_num = SD_MISO_PIN,
        .sclk_io_num = SD_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    // Initialize SPI bus (VSPI_HOST for SD card)
    ret = spi_bus_initialize(VSPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize VSPI bus: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // SD card SPI device configuration
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CS_PIN;
    slot_config.host_id = VSPI_HOST;
    
    // Mount filesystem
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = VSPI_HOST;
    
    ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slot_config, 
                                   &mount_config, &sd_card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGW(TAG, "Failed to mount SD card filesystem. Is card inserted?");
        } else {
            ESP_LOGE(TAG, "Failed to mount SD card: %s", esp_err_to_name(ret));
        }
        return ret;
    }
    
    ESP_LOGI(TAG, "SD card mounted successfully");
    
    // Print card info
    sdmmc_card_print_info(stdout, sd_card);
    
    // Test write and read
    ESP_LOGI(TAG, "Testing SD card write/read...");
    FILE *f = fopen(SD_MOUNT_POINT "/test.txt", "w");
    if (f) {
        fprintf(f, "Lindi SD Card Test\n");
        fprintf(f, "Build: %s %s\n", __DATE__, __TIME__);
        fclose(f);
        ESP_LOGI(TAG, "File written successfully");
        
        // Read back
        char line[128];
        f = fopen(SD_MOUNT_POINT "/test.txt", "r");
        if (f) {
            while (fgets(line, sizeof(line), f) != NULL) {
                // Remove newline
                line[strcspn(line, "\n")] = 0;
                ESP_LOGI(TAG, "Read from SD: %s", line);
            }
            fclose(f);
        }
    } else {
        ESP_LOGW(TAG, "Failed to open file for writing");
    }
    
    return ESP_OK;
}

// Note: Digital clock mode NVS functions removed - now handled by clock_component.c

// Initialize I2C master
static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,  // Using external 4.7kΩ pull-ups
        .scl_pullup_en = GPIO_PULLUP_DISABLE,  // Using external 4.7kΩ pull-ups
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(err));
        return err;
    }
    
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(err));
        return err;
    }
    
    ESP_LOGI(TAG, "I2C master initialized on SDA=%d, SCL=%d", I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    return ESP_OK;
}

// Scan I2C bus for devices
static void i2c_scan(void)
{
    ESP_LOGI(TAG, "Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Found device at address 0x%02X", addr);
        }
    }
    ESP_LOGI(TAG, "I2C scan complete");
}

// Initialize MPU6050
static esp_err_t mpu6050_init(void)
{
    esp_err_t err;
    uint8_t who_am_i;
    
    // Scan bus first to find devices
    i2c_scan();
    
    // Try address 0x68 first
    err = i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                       (uint8_t[]){MPU6050_WHO_AM_I}, 1,
                                       &who_am_i, 1,
                                       1000 / portTICK_PERIOD_MS);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 not found at 0x68, trying 0x69...");
        // Try alternate address (AD0=VCC)
        err = i2c_master_write_read_device(I2C_MASTER_NUM, 0x69,
                                           (uint8_t[]){MPU6050_WHO_AM_I}, 1,
                                           &who_am_i, 1,
                                           1000 / portTICK_PERIOD_MS);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "MPU6050 found at alternate address 0x69!");
            // Update the address for future use
            ESP_LOGW(TAG, "Please update MPU6050_ADDR to 0x69 in code");
        } else {
            ESP_LOGE(TAG, "MPU6050 not found at either 0x68 or 0x69: %s", esp_err_to_name(err));
            return err;
        }
    }
    
    // Accept both original (0x68) and clone (0x70) WHO_AM_I values
    if (who_am_i != 0x68 && who_am_i != 0x70) {
        ESP_LOGE(TAG, "MPU6050 WHO_AM_I mismatch: 0x%02X (expected 0x68 or 0x70)", who_am_i);
        return ESP_ERR_NOT_FOUND;
    }
    
    ESP_LOGI(TAG, "MPU6050 detected! WHO_AM_I=0x%02X", who_am_i);
    
    // Wake up MPU6050 (exit sleep mode)
    uint8_t pwr_mgmt = 0x00;
    err = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                     (uint8_t[]){MPU6050_PWR_MGMT_1, pwr_mgmt}, 2,
                                     1000 / portTICK_PERIOD_MS);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to wake up MPU6050: %s", esp_err_to_name(err));
        return err;
    }
    
    ESP_LOGI(TAG, "MPU6050 initialized successfully");
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Give sensor time to stabilize
    
    return ESP_OK;
}

// MPU6050 reading task - outputs data every second
#if USE_FAKE_SENSOR_DATA
// Fake sensor data generator for UI testing
// Generates smooth sinusoidal pitch/roll values, updates 3 times per second
static void fake_sensor_task(void *pvParameters)
{
    (void)pvParameters;
    float phase = 0.0f;
    const float phase_increment = 0.15f;  // Controls speed of oscillation
    
    ESP_LOGI(TAG, "Fake sensor task started - 30 updates/sec");
    
    while (1) {
        // Generate fake pitch and roll values within ±15° range
        // Using sine waves with different frequencies for variety
        float fake_pitch = 12.0f * sinf(phase);           // ±12° pitch
        float fake_roll = 10.0f * sinf(phase * 1.3f);     // ±10° roll, slightly different frequency
        
        // Update global values with mutex
        if (xSemaphoreTake(mpu_mutex, portMAX_DELAY)) {
            current_pitch = fake_pitch;
            current_roll = fake_roll;
            xSemaphoreGive(mpu_mutex);
        }
        
        phase += phase_increment;
        if (phase > 2.0f * M_PI) {
            phase -= 2.0f * M_PI;  // Keep phase bounded
        }
        
        vTaskDelay(33 / portTICK_PERIOD_MS);  // Update 30 times per second (~30Hz)
    }
}
#else
// Real MPU6050 sensor reading task
static void mpu6050_read_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t data[14];  // Read all sensor data in one transaction
    
    ESP_LOGI(TAG, "MPU6050 read task started");
    
    while (1) {
        // Read all sensor data (accel + temp + gyro) in one burst read
        // Registers: 0x3B-0x48 (14 bytes total)
        esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                                     (uint8_t[]){MPU6050_ACCEL_XOUT_H}, 1,
                                                     data, 14,
                                                     100 / portTICK_PERIOD_MS);
        
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read MPU6050: %s", esp_err_to_name(err));
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        
        // Parse raw data from the 14-byte buffer
        // Accel: bytes 0-5
        int16_t accel_x_raw = (int16_t)((data[0] << 8) | data[1]);
        int16_t accel_y_raw = (int16_t)((data[2] << 8) | data[3]);
        int16_t accel_z_raw = (int16_t)((data[4] << 8) | data[5]);
        
        // Temp: bytes 6-7
        int16_t temp_raw = (int16_t)((data[6] << 8) | data[7]);
        
        // Gyro: bytes 8-13
        int16_t gyro_x_raw = (int16_t)((data[8] << 8) | data[9]);
        int16_t gyro_y_raw = (int16_t)((data[10] << 8) | data[11]);
        int16_t gyro_z_raw = (int16_t)((data[12] << 8) | data[13]);
        float temperature = (temp_raw / 340.0f) + 36.53f;
        
        // Convert to physical units
        float ax = accel_x_raw / 16384.0f;
        float ay = accel_y_raw / 16384.0f;
        float az = accel_z_raw / 16384.0f;
        
        float gx = gyro_x_raw / 131.0f;
        float gy = gyro_y_raw / 131.0f;
        float gz = gyro_z_raw / 131.0f;
        
        // Output raw data stream
        printf("MPU6050 RAW | AccXYZ: %6d %6d %6d | GyroXYZ: %6d %6d %6d | Temp: %6d (%.1f°C)\n",
               accel_x_raw, accel_y_raw, accel_z_raw,
               gyro_x_raw, gyro_y_raw, gyro_z_raw,
               temp_raw, temperature);
        
        // Raw sensor data (no longer printed to reduce serial spam)
        // printf("MPU6050  G  | AccXYZ: %6.2f %6.2f %6.2f | GyroXYZ: %6.1f %6.1f %6.1f\n",
        //        ax, ay, az, gx, gy, gz);
        
        // Calculate angles from accelerometer (in degrees)
        // Note: Due to physical sensor mounting orientation:
        // - Sensor's mathematical 'pitch' axis = Physical ROLL (left/right tilt)
        // - Sensor's mathematical 'roll' axis = Physical PITCH (forward/backward tilt)
        float sensor_pitch_axis = -atan2(ay, sqrt(ax * ax + az * az)) * 180.0f / M_PI;
        float sensor_roll_axis = -atan2(ax, az) * 180.0f / M_PI;
        
        // Map to physical orientation (sensor mounted 90° rotated)
        float physical_pitch = sensor_roll_axis;   // Forward/backward tilt
        float physical_roll = sensor_pitch_axis;   // Left/right tilt
        
        // Apply calibration offsets (subtract to zero out)
        physical_pitch -= pitch_offset;
        physical_roll -= roll_offset;
        
        // Update global values with mutex
        if (xSemaphoreTake(mpu_mutex, portMAX_DELAY)) {
            current_pitch = physical_pitch;
            current_roll = physical_roll;
            xSemaphoreGive(mpu_mutex);
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms = 10Hz polling
    }
}
#endif

// Initialize WiFi in station mode
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "WiFi init finished, waiting for connection...");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler() */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to WiFi SSID:%s", WIFI_SSID);
        // Initialize SNTP after WiFi connects
        initialize_sntp();
        
        // Wait for time to be set
        time_t now = 0;
        struct tm timeinfo = { 0 };
        int retry = 0;
        const int retry_count = 10;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        time(&now);
        localtime_r(&now, &timeinfo);
        
        // Set timezone to Amsterdam (CET/CEST)
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();
        
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "Unexpected WiFi event");
    }
}


// Main function
void app_main() {
	ESP_LOGI(TAG, "Starting...");
	
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	// Load winter time setting from NVS
	load_winter_time_setting();
	
	// Load dark theme setting from NVS
	load_dark_theme_setting();
	
	// Load sensor inversion setting from NVS
	load_sensor_inversion_setting();
	
	// Load language setting from NVS
	load_language_setting();
	
	// Load calibration offsets from NVS
	load_calibration_offsets();
	
	// Initialize event loop
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	
	// Initialize I2C
	ESP_LOGI(TAG, "Initializing I2C...");
	ESP_ERROR_CHECK(i2c_master_init());
	
#if USE_FAKE_SENSOR_DATA
	// Using fake sensor data for UI testing
	ESP_LOGI(TAG, "Using FAKE sensor data (MPU6050 disabled)");
	mpu_mutex = xSemaphoreCreateMutex();
	xTaskCreatePinnedToCore(fake_sensor_task, "fake_sensor", 2048, NULL, 5, NULL, 0);
#else
	// Initialize real MPU6050
	ESP_LOGI(TAG, "Initializing MPU6050...");
	if (mpu6050_init() == ESP_OK) {
		// Create mutex for MPU data
		mpu_mutex = xSemaphoreCreateMutex();
		
		// Start MPU6050 reading task - pinned to Core 0 to avoid blocking display
		xTaskCreatePinnedToCore(mpu6050_read_task, "mpu6050_read", 4096, NULL, 5, NULL, 0);
		ESP_LOGI(TAG, "MPU6050 task started on Core 0");
	} else {
		ESP_LOGW(TAG, "MPU6050 initialization failed, continuing without sensor");
	}
#endif
	
	// Initialize WiFi with power save mode
	ESP_LOGI(TAG, "Starting WiFi...");
	wifi_init_sta();
	
	// Initialize SD card (optional - continues if card not present)
	// TEMPORARILY DISABLED - May conflict with display SPI
	// ESP_LOGI(TAG, "Checking for SD card...");
	// ret = init_sd_card();
	// if (ret == ESP_OK) {
	// 	ESP_LOGI(TAG, "SD card is accessible and ready");
	// } else {
	// 	ESP_LOGW(TAG, "SD card not available (insert card and restart if needed)");
	// }
	
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// GUI task pinned to Core 1 - keeps display smooth while Core 0 handles I2C
	xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);
}

static void lv_tick_task(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

//Creates a semaphore to handle concurrent call to lvgl stuff
//If you wish to call *any* lvgl function from other threads/tasks
//you should lock on the very same semaphore!
SemaphoreHandle_t xGuiSemaphore;		// 创建一个GUI信号量
static bool perf_monitor_hidden = false;	// Flag to track if we've hidden the perf monitor

// Custom formatter for clock hour labels (unused - kept for reference)
// Gauge places labels at values 0,5,10,15,20,25,30,35,40,45,50,55
static void clock_label_formatter(lv_obj_t *gauge, char *buf, int bufsize, int32_t value)
{
	// Map gauge position to clock hour
	// +7 offset positions "12" at the top (0°) of the dial
	int hour = ((value / 5) + 7) % 12;
	if (hour == 0) {
		snprintf(buf, bufsize, "12");
	} else {
		snprintf(buf, bufsize, "%d", hour);
	}
}

// Map angle to bar value with logarithmic scaling
// 75% of bar for ±10°, 25% for 10-30°
static int16_t map_angle_to_bar(float angle)
{
	float abs_angle = fabsf(angle);
	float sign = (angle >= 0) ? 1.0f : -1.0f;
	float mapped;
	
	if (abs_angle <= 10.0f) {
		// Linear mapping for ±10°: use 75% of range (22.5 out of 30)
		mapped = (abs_angle / 10.0f) * 22.5f;
	} else {
		// Linear mapping for 10-30°: use remaining 25% (7.5 out of 30)
		mapped = 22.5f + ((abs_angle - 10.0f) / 20.0f) * 7.5f;
	}
	
	return (int16_t)(sign * mapped);
}

// Level menu update task - updates bars with logarithmic mapping
static void level_menu_update_task(lv_task_t *task)
{
	(void)task;
	
	// Check if UI and mutex are ready
	if (!pitch_bar || !roll_bar || !pitch_label || !roll_label || !mpu_mutex) {
		return;  // Not ready yet
	}
	
	float pitch, roll;
	
	// Get current values with mutex
	if (xSemaphoreTake(mpu_mutex, 10 / portTICK_PERIOD_MS)) {
		pitch = current_pitch;
		roll = current_roll;
		xSemaphoreGive(mpu_mutex);
	} else {
		return;  // Couldn't get mutex, skip this update
	}
	
	// Apply sensor inversion if enabled (for backward-mounted sensor)
	if (sensor_inverted) {
		pitch = -pitch;
		roll = -roll;
	}
	
	// Clamp to ±30 degrees and use raw 1:1 mapping (no logarithmic transform)
	if (pitch > 30.0f) pitch = 30.0f;
	if (pitch < -30.0f) pitch = -30.0f;
	if (roll > 30.0f) roll = 30.0f;
	if (roll < -30.0f) roll = -30.0f;
	
	// Direct 1:1 mapping: degrees to bar units
	int16_t pitch_mapped = (int16_t)pitch;
	int16_t roll_mapped = (int16_t)roll;
	
	// Only update UI if values have actually changed (prevents unnecessary redraws)
	if (pitch_mapped != prev_pitch_mapped) {
		lv_bar_set_start_value(pitch_bar, pitch_mapped - 2, LV_ANIM_OFF);
		lv_bar_set_value(pitch_bar, pitch_mapped + 2, LV_ANIM_OFF);
		
		char pitch_text[32];
		snprintf(pitch_text, sizeof(pitch_text), "%s: %.1f°", STR_PITCH[current_language], pitch);
		lv_label_set_text(pitch_label, pitch_text);
		
		prev_pitch_mapped = pitch_mapped;
	}
	
	if (roll_mapped != prev_roll_mapped) {
		lv_bar_set_start_value(roll_bar, roll_mapped - 2, LV_ANIM_OFF);
		lv_bar_set_value(roll_bar, roll_mapped + 2, LV_ANIM_OFF);
		
		char roll_text[32];
		snprintf(roll_text, sizeof(roll_text), "%s: %.1f°", STR_ROLL[current_language], roll);
		lv_label_set_text(roll_label, roll_text);
		
		prev_roll_mapped = roll_mapped;
	}
}

void guiTask(void *pvParameter) {
    
    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();    // 创建GUI信号量
    lv_init();          // 初始化LittlevGL
    lvgl_driver_init(); // 初始化液晶SPI驱动 触摸芯片SPI/IIC驱动

    static lv_color_t buf1[DISP_BUF_SIZE];
#ifndef CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    static lv_color_t buf2[DISP_BUF_SIZE];
#endif
    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_IL3820 
    /* Actual size in pixel, not bytes and use single buffer */
    size_in_px *= 8;
    lv_disp_buf_init(&disp_buf, buf1, NULL, size_in_px);
#elif defined CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    lv_disp_buf_init(&disp_buf, buf1, NULL, size_in_px);
#else
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
#endif

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

// 如果配置为 单色模式
#ifdef CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);


// 如果有配置触摸芯片，配置触摸
#if CONFIG_LVGL_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif


    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // 一个标签演示
    //lv_obj_t * scr = lv_disp_get_scr_act(NULL);         // 获取当前屏幕
    //lv_obj_t * label1 =  lv_label_create(scr, NULL);    // 在当前活动的屏幕上创建标签
    //lv_label_set_text(label1, "Hello\nworld!");         // 修改标签的文字
    // 对象对齐函数，将标签中心对齐，NULL表示在父级上对齐，当前父级是屏幕，0，0表示对齐后的x，y偏移量
    //lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
/*
	lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);
	lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL); //Create an image object for the cursor 
	lv_img_set_src(cursor_obj, &mouse_cursor_icon);             //Set the image source
	lv_indev_set_cursor(mouse_indev, cursor_obj);               //Connect the image  object to the driver
*/	
	// Create tabview with 3 tabs: Start, Level, Info
	lv_obj_t *tv = lv_tabview_create(lv_scr_act(), NULL);
	lv_obj_t *tab_start = lv_tabview_add_tab(tv, STR_TAB_START[current_language]);
	lv_obj_t *tab_level = lv_tabview_add_tab(tv, STR_TAB_LEVEL[current_language]);
	lv_obj_t *tab_info = lv_tabview_add_tab(tv, STR_TAB_INFO[current_language]);
	
	// Make Start and Level tabs non-scrollable
	lv_page_set_scrl_layout(tab_start, LV_LAYOUT_OFF);
	lv_page_set_scrl_layout(tab_level, LV_LAYOUT_OFF);
	lv_page_set_scrlbar_mode(tab_level, LV_SCRLBAR_MODE_OFF);  // Hide scrollbar
	
	// Create clock component (handles both analog and digital clocks)
	clock_config_t clock_cfg = {
		.parent = tab_start,
		.x_offset = 0,
		.y_offset = 0,
		.start_with_digital = false,  // Default to analog (NVS will override if saved)
		.show_toggle_button = true
	};
	main_clock = clock_create(&clock_cfg);
	if (!main_clock) {
		ESP_LOGE(TAG, "Failed to create clock component");
	}
	
	// Create clock update task (1 second interval)
	lv_task_create(clock_update_task, 1000, LV_TASK_PRIO_LOW, NULL);
	
	// Create Level menu update task (100ms = 10Hz, sufficient for level display)
	lv_task_create(level_menu_update_task, 100, LV_TASK_PRIO_MID, NULL);
	
	// Add content to Level tab
	// Create Pitch bar (vertical, centered top)
	pitch_bar = lv_bar_create(tab_level, NULL);
	lv_obj_set_size(pitch_bar, 15, 90);  // Thinner: 15px wide, 90px high
	lv_obj_align(pitch_bar, NULL, LV_ALIGN_CENTER, 0, -25);  // Centered, shifted up
	lv_bar_set_range(pitch_bar, -30, 30);  // +/- 30 degrees max
	lv_bar_set_start_value(pitch_bar, -2, LV_ANIM_OFF);  // Thicker indicator line
	lv_bar_set_value(pitch_bar, 2, LV_ANIM_OFF);
	
	// Pitch label
	pitch_label = lv_label_create(tab_level, NULL);
	char pitch_initial[32];
	snprintf(pitch_initial, sizeof(pitch_initial), "%s: 0°", STR_PITCH[current_language]);
	lv_label_set_text(pitch_label, pitch_initial);
	lv_obj_align(pitch_label, pitch_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);
	
	// Create Roll bar (horizontal, centered below pitch)
	roll_bar = lv_bar_create(tab_level, NULL);
	lv_obj_set_size(roll_bar, 120, 15);  // Horizontal: 120px wide, 15px high
	lv_obj_align(roll_bar, pitch_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_bar_set_range(roll_bar, -30, 30);  // +/- 30 degrees max
	lv_bar_set_start_value(roll_bar, -2, LV_ANIM_OFF);  // Thicker indicator line
	lv_bar_set_value(roll_bar, 2, LV_ANIM_OFF);
	
	// Roll label
	roll_label = lv_label_create(tab_level, NULL);
	char roll_initial[32];
	snprintf(roll_initial, sizeof(roll_initial), "%s: 0°", STR_ROLL[current_language]);
	lv_label_set_text(roll_label, roll_initial);
	lv_obj_align(roll_label, roll_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);
	
	// Add Calibrate button (left side of pitch bar)
	lv_obj_t *btn_calibrate = lv_btn_create(tab_level, NULL);
	lv_obj_set_size(btn_calibrate, 70, 35);
	lv_obj_align(btn_calibrate, pitch_bar, LV_ALIGN_OUT_LEFT_MID, -10, 0);
	lv_obj_set_event_cb(btn_calibrate, calibrate_btn_cb);
	
	lv_obj_t *label_cal = lv_label_create(btn_calibrate, NULL);
	lv_label_set_text(label_cal, STR_CALIBRATE[current_language]);
	
	// Add Reset button (below calibrate button)
	lv_obj_t *btn_reset = lv_btn_create(tab_level, NULL);
	lv_obj_set_size(btn_reset, 70, 35);
	lv_obj_align(btn_reset, btn_calibrate, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	lv_obj_set_event_cb(btn_reset, reset_calibration_cb);
	
	lv_obj_t *label_reset = lv_label_create(btn_reset, NULL);
	lv_label_set_text(label_reset, STR_RESET[current_language]);
	
	// Add content to Info tab
	lv_obj_t *label_info = lv_label_create(tab_info, NULL);
	
	// Build version string from compile date/time
	char version_str[200];
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char month_str[4], day_str[3], year_str[5], time_str[9];
	int month_num = 0;
	
	// Parse __DATE__ (format: "MMM DD YYYY")
	sscanf(__DATE__, "%s %s %s", month_str, day_str, year_str);
	for (int i = 0; i < 12; i++) {
		if (strcmp(month_str, months[i]) == 0) {
			month_num = i + 1;
			break;
		}
	}
	
	// Parse __TIME__ (format: "HH:MM:SS")
	sscanf(__TIME__, "%s", time_str);
	char hhmm[5];
	hhmm[0] = time_str[0];
	hhmm[1] = time_str[1];
	hhmm[2] = time_str[3];
	hhmm[3] = time_str[4];
	hhmm[4] = '\0';
	
	snprintf(version_str, sizeof(version_str), 
	         "Version: DEV%s%02d%s-%s\n\n"
	         "(c) Syquens B.V. 2025\n"
	         "V.N. Verbon",
	         year_str, month_num, day_str, hhmm);
	
	lv_label_set_text(label_info, version_str);
	lv_obj_align(label_info, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
	
	// Add WiFi status display
	lv_obj_t *wifi_label = lv_label_create(tab_info, NULL);
	char wifi_status[100];
	snprintf(wifi_status, sizeof(wifi_status), 
	         "WiFi: %s\nIP: %s", 
	         wifi_connected ? "Connected" : "Disconnected",
	         wifi_ip_addr);
	lv_label_set_text(wifi_label, wifi_status);
	lv_obj_align(wifi_label, label_info, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	// Add timezone selector
	lv_obj_t *tz_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(tz_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(tz_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(tz_cont, wifi_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	tz_label = lv_label_create(tz_cont, NULL);
	lv_label_set_text(tz_label, STR_TIMEZONE[current_language]);
	
	timezone_selector = lv_dropdown_create(tz_cont, NULL);
	lv_dropdown_set_options(timezone_selector, 
	    "GMT-12\nGMT-11\nGMT-10\nGMT-9\nGMT-8\nGMT-7\nGMT-6\nGMT-5\nGMT-4\nGMT-3\nGMT-2\nGMT-1\n"
	    "GMT+0\nGMT+1\nGMT+2\nGMT+3\nGMT+4\nGMT+5\nGMT+6\nGMT+7\nGMT+8\nGMT+9\nGMT+10\nGMT+11\nGMT+12");
	lv_dropdown_set_selected(timezone_selector, 13);  // Default: GMT+1 (Amsterdam)
	lv_obj_set_event_cb(timezone_selector, timezone_selector_cb);
	
	// Add winter time toggle
	lv_obj_t *winter_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(winter_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(winter_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(winter_cont, tz_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	winter_label = lv_label_create(winter_cont, NULL);
	lv_label_set_text(winter_label, STR_WINTER_TIME[current_language]);
	
	lv_obj_t *winter_switch = lv_switch_create(winter_cont, NULL);
	if (winter_time_enabled) {
		lv_switch_on(winter_switch, LV_ANIM_OFF);
	} else {
		lv_switch_off(winter_switch, LV_ANIM_OFF);
	}
	lv_obj_set_event_cb(winter_switch, winter_time_toggle_cb);
	
	// Add performance monitor toggle (scrollable with page)
	lv_obj_t *perf_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(perf_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(perf_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(perf_cont, winter_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	perf_label = lv_label_create(perf_cont, NULL);
	lv_label_set_text(perf_label, STR_SHOW_FPS[current_language]);
	
	lv_obj_t *perf_switch = lv_switch_create(perf_cont, NULL);
	lv_switch_off(perf_switch, LV_ANIM_OFF);  // OFF by default
	lv_obj_set_event_cb(perf_switch, perf_monitor_toggle_cb);
	
	// Add dark theme toggle
	lv_obj_t *theme_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(theme_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(theme_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(theme_cont, perf_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	theme_label = lv_label_create(theme_cont, NULL);
	lv_label_set_text(theme_label, STR_DARK_THEME[current_language]);
	
	lv_obj_t *theme_switch = lv_switch_create(theme_cont, NULL);
	if (dark_theme_enabled) {
		lv_switch_on(theme_switch, LV_ANIM_OFF);
		// Apply dark theme on startup
		uint32_t flags = LV_THEME_MATERIAL_FLAG_DARK;
		lv_theme_t *th = lv_theme_material_init(LV_THEME_DEFAULT_COLOR_PRIMARY, 
		                                         LV_THEME_DEFAULT_COLOR_SECONDARY,
		                                         flags,
		                                         LV_THEME_DEFAULT_FONT_SMALL, 
		                                         LV_THEME_DEFAULT_FONT_NORMAL, 
		                                         LV_THEME_DEFAULT_FONT_SUBTITLE, 
		                                         LV_THEME_DEFAULT_FONT_TITLE);
		lv_theme_set_act(th);
	} else {
		lv_switch_off(theme_switch, LV_ANIM_OFF);
	}
	lv_obj_set_event_cb(theme_switch, dark_theme_toggle_cb);
	
	// Sensor orientation inversion toggle (for backward-mounted MPU6050)
	lv_obj_t *sensor_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(sensor_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(sensor_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(sensor_cont, theme_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	sensor_label = lv_label_create(sensor_cont, NULL);
	lv_label_set_text(sensor_label, STR_INVERT_LEVEL[current_language]);
	
	lv_obj_t *sensor_switch = lv_switch_create(sensor_cont, NULL);
	if (sensor_inverted) {
		lv_switch_on(sensor_switch, LV_ANIM_OFF);
	} else {
		lv_switch_off(sensor_switch, LV_ANIM_OFF);
	}
	lv_obj_set_event_cb(sensor_switch, sensor_inversion_toggle_cb);

	// Language selection toggle (EN/NL)
	lv_obj_t *lang_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(lang_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(lang_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(lang_cont, sensor_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	lang_label = lv_label_create(lang_cont, NULL);
	lv_label_set_text(lang_label, STR_LANGUAGE[current_language]);
	
	lv_obj_t *lang_switch = lv_switch_create(lang_cont, NULL);
	if (current_language == LANG_NL) {
		lv_switch_on(lang_switch, LV_ANIM_OFF);
	} else {
		lv_switch_off(lang_switch, LV_ANIM_OFF);
	}
	lv_obj_set_event_cb(lang_switch, language_toggle_cb);

    while (1) {
		vTaskDelay(1);
		// 尝试锁定信号量，如果成功，请调用lvgl的东西
		if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
            lv_task_handler();
            
            // Hide performance monitor on first render (it's created by LVGL after first refresh)
            if (!perf_monitor_hidden) {
                lv_obj_t *sys_layer = lv_layer_sys();
                if (sys_layer) {
                    lv_obj_t *child = lv_obj_get_child(sys_layer, NULL);
                    if (child != NULL) {
                        lv_obj_set_hidden(child, true);
                        perf_monitor_hidden = true;
                    }
                }
            }
            
            xSemaphoreGive(xGuiSemaphore);  // 释放信号量
        }
    }
    vTaskDelete(NULL);      // 删除任务
}

// Callback to toggle FPS/CPU performance monitor
static void perf_monitor_toggle_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        // Find the performance label on the system layer
        lv_obj_t *sys_layer = lv_layer_sys();
        if (sys_layer) {
            // Iterate through children to find the perf label
            lv_obj_t *child = lv_obj_get_child(sys_layer, NULL);
            while (child != NULL) {
                // The performance monitor label is the only child on system layer
                // Toggle visibility
                if (lv_switch_get_state(sw)) {
                    lv_obj_set_hidden(child, false);
                } else {
                    lv_obj_set_hidden(child, true);
                }
                child = lv_obj_get_child(sys_layer, child);
            }
        }
    }
}

// Callback for timezone selector dropdown
static void timezone_selector_cb(lv_obj_t *dd, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        uint16_t selected = lv_dropdown_get_selected(dd);
        // Convert dropdown index to timezone offset
        // GMT-12 is index 0, GMT+0 is index 12, GMT+12 is index 24
        timezone_offset = (int)selected - 12;
        ESP_LOGI(TAG, "Timezone changed to GMT%+d", timezone_offset);
    }
}

// Callback for winter time toggle
static void winter_time_toggle_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        winter_time_enabled = lv_switch_get_state(sw);
        save_winter_time_setting(winter_time_enabled);
        ESP_LOGI(TAG, "Winter time %s", winter_time_enabled ? "enabled" : "disabled");
    }
}

// Callback for dark theme toggle
static void dark_theme_toggle_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        dark_theme_enabled = lv_switch_get_state(sw);
        save_dark_theme_setting(dark_theme_enabled);
        uint32_t flags = dark_theme_enabled ? LV_THEME_MATERIAL_FLAG_DARK : LV_THEME_MATERIAL_FLAG_LIGHT;
        lv_theme_t *th = lv_theme_material_init(LV_THEME_DEFAULT_COLOR_PRIMARY, 
                                                 LV_THEME_DEFAULT_COLOR_SECONDARY,
                                                 flags,
                                                 LV_THEME_DEFAULT_FONT_SMALL, 
                                                 LV_THEME_DEFAULT_FONT_NORMAL, 
                                                 LV_THEME_DEFAULT_FONT_SUBTITLE, 
                                                 LV_THEME_DEFAULT_FONT_TITLE);
        lv_theme_set_act(th);
        ESP_LOGI(TAG, "Theme changed to %s", dark_theme_enabled ? "dark" : "light");
    }
}

// Callback for sensor inversion toggle
static void sensor_inversion_toggle_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        sensor_inverted = lv_switch_get_state(sw);
        save_sensor_inversion_setting(sensor_inverted);
        ESP_LOGI(TAG, "Sensor orientation: %s", sensor_inverted ? "inverted (pins backward)" : "normal (pins forward)");
    }
}

// Callback for language toggle
static void language_toggle_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        bool is_nl = lv_switch_get_state(sw);
        current_language = is_nl ? LANG_NL : LANG_EN;
        save_language_setting(current_language);
        ESP_LOGI(TAG, "Language changed to: %s", current_language == LANG_NL ? "NL" : "EN");
        
        // Update all Info tab labels dynamically
        if (tz_label) lv_label_set_text(tz_label, STR_TIMEZONE[current_language]);
        if (winter_label) lv_label_set_text(winter_label, STR_WINTER_TIME[current_language]);
        if (perf_label) lv_label_set_text(perf_label, STR_SHOW_FPS[current_language]);
        if (theme_label) lv_label_set_text(theme_label, STR_DARK_THEME[current_language]);
        if (sensor_label) lv_label_set_text(sensor_label, STR_INVERT_LEVEL[current_language]);
        if (lang_label) lv_label_set_text(lang_label, STR_LANGUAGE[current_language]);
        
        // Note: Tab names require restart to update
    }
}

// Confirmation dialog callback for calibration
static void calibrate_confirm_cb(lv_obj_t *btnm, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        uint16_t btn_id = lv_btnmatrix_get_active_btn(btnm);
        lv_obj_t *mbox = lv_obj_get_parent(btnm);
        
        if (btn_id == 0) {  // Yes button
            // Perform calibration - save current sensor readings as offsets
            if (xSemaphoreTake(mpu_mutex, 100 / portTICK_PERIOD_MS)) {
                pitch_offset = current_pitch;
                roll_offset = current_roll;
                xSemaphoreGive(mpu_mutex);
                
                save_calibration_offsets(pitch_offset, roll_offset);
                ESP_LOGI(TAG, "Calibrated! Offsets: pitch=%.3f° roll=%.3f°", pitch_offset, roll_offset);
            }
        } else {  // No button
            ESP_LOGI(TAG, "Calibration cancelled");
        }
        
        // Close the message box
        lv_msgbox_start_auto_close(mbox, 0);
    }
}

// Cancel callback for calibration (not used, kept for compatibility)
static void calibrate_cancel_cb(lv_obj_t *btn, lv_event_t e)
{
    (void)btn;
    (void)e;
}

// Calibration button callback - shows confirmation dialog
static void calibrate_btn_cb(lv_obj_t *btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        // Create confirmation message box
        static const char *btns[] = {NULL, NULL, ""};  // Will be filled with translated strings
        btns[0] = STR_YES[current_language];
        btns[1] = STR_NO[current_language];
        
        char msg[150];
        snprintf(msg, sizeof(msg), "%s\n\n%s", 
                 STR_CONFIRM_CAL[current_language],
                 STR_CAL_WARNING[current_language]);
        
        lv_obj_t *mbox = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text(mbox, msg);
        lv_msgbox_add_btns(mbox, btns);
        lv_obj_set_width(mbox, 250);
        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
        
        // Set event callback for button matrix
        lv_obj_t *btnm = lv_msgbox_get_btnmatrix(mbox);
        lv_obj_set_event_cb(btnm, calibrate_confirm_cb);
    }
}

// Reset calibration button callback
static void reset_calibration_cb(lv_obj_t *btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        reset_calibration_offsets();
        ESP_LOGI(TAG, "Calibration reset");
    }
}

// Note: digital_clock_toggle_cb removed - now handled by clock_component.c

// Clock update task - called every second
// Note: This is called from LVGL task context, so semaphore is already held
static void clock_update_task(lv_task_t *task)
{
    (void)task;
    
    if (!main_clock) {
        return;  // Clock not initialized yet
    }
    
    // Get current time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Update clock component (handles both analog and digital displays)
    clock_update(main_clock, &timeinfo);
}
