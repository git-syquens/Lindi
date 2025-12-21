/* LVGL Example project

液晶驱动芯片支持
ILI9341 240*320 3.2     14-25FPS
ILI9488 320*480 3.5     3-7FPS

ENC28J60_INT	GPIO34
ENC28J60_CS		GPIO5
FLASH_CS		GPIO21
SD_CS			GPIO22

VSPI SPI3_HOST=2	SD、FALASH、ENC28J60
CS0		GPIO_5
SCLK	GPIO_18
MISO	GPIO_19
MOSI	GPIO_23
QUADWP	GPIO_22
QUADHD	GPIO_21

HSPI SPI2_HOST=1   液晶独用
CS0		GPIO_15
SCLK	GPIO_14
MISO	GPIO_12
MOSI	GPIO_13
QUADWP	GPIO_2
QUADHD	GPIO_4


液晶SPI引脚定义  HSPI    SPI2_HOST=1
omponent config → LVGL TFT Display controller → Display Pin Assignments
MOSI (Master Out Slave In)			GPIO_13
MISO (Slave Out Master In)			None		GPIO_12
CLK (SCK / Serial Clock)			GPIO_14
CS (Slave Select)					GPIO_15
DC (Data / Command)					GPIO_2
Reset								GOIO_4
Backlight							None

触摸SPI引脚定义components\lvgl_esp32_drivers\lvgl_touch\xpt2046.h
omponent config → LVGL Touch controller → Touchpanel (XPT2046) Pin Assignments
MISO (Master In Slave Out)			GPIO_39(SENSOR_VN只能输入)
MOSI (Master Out Slave In)			GPIO_32
CLK (SCK / Serial Clock)			GPIO_33
CS (Slave Select)					GPIO_25
IRQ (Interrupt Request)				GPIO_36(SENSOR_VP只能输入)      


==================================================模块左==================================================
3V3			3V3			2		供电
复位按钮	 EN			 3		高电平：芯片使能/低电平：芯片关闭/注意：不能让 EN 管脚浮空
TP_IRQ		SENSOR_VP	4		GPIO36, ADC1_CH0, RTC_GPIO0			只能输入
TP_MISO		SENSOR_VN	5		GPIO39, ADC1_CH3, RTC_GPIO3			只能输入
RLight		IO34		6		GPIO34, ADC1_CH6, RTC_GPIO4			只能输入
IRRev		IO35		7		GPIO35, ADC1_CH7, RTC_GPIO5			只能输入
TP_MOSI		IO32		8		GPIO32, XTAL_32K_P (32.768 kHz 晶振输入), ADC1_CH4, TOUCH9,RTC_GPIO9
TP_CS		IO33		9		GPIO33, XTAL_32K_N (32.768 kHz 晶振输出), ADC1_CH5, TOUCH8,RTC_GPIO8
TP_CLK		IO25		10		GPIO25, DAC_1, ADC2_CH8, RTC_GPIO6, EMAC_RXD0
DHT11		IO26		11		GPIO26, DAC_2, ADC2_CH9, RTC_GPIO7, EMAC_RXD1
TouchPad	IO27		12		GPIO27, ADC2_CH7, TOUCH7, RTC_GPIO17, EMAC_RX_DV
TFT_CLK		IO14		13		GPIO14, ADC2_CH6, TOUCH6, RTC_GPIO16, MTMS, HSPICLK,HS2_CLK, SD_CLK, EMAC_TXD2
TFT_MISO	IO12		14		GPIO12, ADC2_CH5, TOUCH5, RTC_GPIO15, MTDI, HSPIQ,HS2_DATA2, SD_DATA2, EMAC_TXD3

================================================模块下==================================================
GND			GND			15		接地
TFT_MOSI	IO13		16		GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER
XXXX		SD2			17		NC
XXXX		SD3			18		NC
XXXX		CMD			19		NC
XXXX		CLK			20		NC
XXXX		SD0			21		NC
XXXX		SD1			22		NC
TFT_CS		IO15		23		GPIO15, ADC2_CH3, TOUCH3, MTDO, HSPICS0, RTC_GPIO13,HS2_CMD, SD_CMD, EMAC_RXD3
TFT_DC		IO2			24		GPIO2, ADC2_CH2, TOUCH2, RTC_GPIO12, HSPIWP, HS2_DATA0,SD_DATA0

================================================模块右==================================================
GND			GND			38		接地
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
Flash按钮	IO0			25		GPIO0, ADC2_CH1, TOUCH1, RTC_GPIO11, CLK_OUT1,EMAC_TX_CLK

 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

// Littlevgl 头文件
#include "lvgl/lvgl.h"			// LVGL头文件
#include "lvgl_helpers.h"		// 助手 硬件驱动相关

#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"

// WiFi Configuration
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASS      "YOUR_WIFI_PASSWORD"
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

#define NVS_NAMESPACE "lindi_cfg"

//LV_IMG_DECLARE(mouse_cursor_icon);			/*Declare the image file.*/


/*********************
 *      DEFINES
 *********************/
#define TAG " LittlevGL Demo"
#define LV_TICK_PERIOD_MS 10

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
void guiTask(void *pvParameter);				// GUI任务
static void perf_monitor_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void clock_update_task(lv_task_t *task);
static void timezone_selector_cb(lv_obj_t *dd, lv_event_t e);
static void winter_time_toggle_cb(lv_obj_t *sw, lv_event_t e);
static void dark_theme_toggle_cb(lv_obj_t *sw, lv_event_t e);

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


// 主函数
void app_main() {
	printf("\r\nAPP %s is start!~\r\n", TAG);
	
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
	
	// Initialize event loop
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	
	// Initialize WiFi with power save mode
	ESP_LOGI(TAG, "Starting WiFi...");
	wifi_init_sta();
	
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// 如果要使用任务创建图形，则需要创建固定核心任务,否则可能会出现诸如内存损坏等问题
	xTaskCreate(guiTask, "gui", 4096*2, NULL, 0, NULL);
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

// Clock variables
static lv_obj_t *clock_label = NULL;
static lv_obj_t *analog_clock_gauge = NULL;

// Custom formatter for clock hour labels
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
	lv_obj_t *tab_start = lv_tabview_add_tab(tv, "Start");
	lv_obj_t *tab_level = lv_tabview_add_tab(tv, "Level");
	lv_obj_t *tab_info = lv_tabview_add_tab(tv, "Info");
	
	// Add digital clock to Start tab
	clock_label = lv_label_create(tab_start, NULL);
	lv_label_set_text(clock_label, "--:--:--");
	// Use larger text by repeating the text with line breaks to make it visually bigger
	lv_label_set_text(clock_label, "--:--:--");
	lv_obj_align(clock_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
	
	// Add analog clock gauge below digital clock
	analog_clock_gauge = lv_gauge_create(tab_start, NULL);
	lv_obj_set_size(analog_clock_gauge, 180, 180);
	lv_obj_align(analog_clock_gauge, clock_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	// Configure gauge for clock: 330° arc (11/12 of circle) to ensure all 12 labels are drawn
	// With 360° the first/last label overlap, causing LVGL to skip one
	lv_gauge_set_scale(analog_clock_gauge, 330, 12, 12);  // 330° arc, 12 tick lines, 12 labels
	lv_gauge_set_range(analog_clock_gauge, 0, 59);         // 0-59 range
	lv_gauge_set_angle_offset(analog_clock_gauge, 225);    // Rotate to put 12 at top (0°), gap at bottom
	
	// Set up 3 needles: hour (white), minute (gray), second (red)
	static lv_color_t needle_colors[3];
	needle_colors[0] = LV_COLOR_MAKE(200, 200, 200);  // Hour hand - light gray
	needle_colors[1] = LV_COLOR_MAKE(150, 150, 150);  // Minute hand - darker gray  
	needle_colors[2] = LV_COLOR_RED;                   // Second hand - red
	lv_gauge_set_needle_count(analog_clock_gauge, 3, needle_colors);
	
	// Set custom label formatter to show clock hours (12, 1, 2, 3...11)
	lv_gauge_set_formatter_cb(analog_clock_gauge, clock_label_formatter);
	
	// Set initial values to 12:00:00
	lv_gauge_set_value(analog_clock_gauge, 0, 0);   // Hour at 12
	lv_gauge_set_value(analog_clock_gauge, 1, 0);   // Minute at 12
	lv_gauge_set_value(analog_clock_gauge, 2, 0);   // Second at 12
	
	// Create clock update task (1 second interval)
	lv_task_create(clock_update_task, 1000, LV_TASK_PRIO_LOW, NULL);
	
	// Add content to Level tab
	lv_obj_t *label_level = lv_label_create(tab_level, NULL);
	lv_label_set_text(label_level, "Level settings");
	lv_obj_align(label_level, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
	
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
	
	lv_obj_t *tz_label = lv_label_create(tz_cont, NULL);
	lv_label_set_text(tz_label, "Timezone:");
	
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
	
	lv_obj_t *winter_label = lv_label_create(winter_cont, NULL);
	lv_label_set_text(winter_label, "Winter Time");
	
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
	
	lv_obj_t *perf_label = lv_label_create(perf_cont, NULL);
	lv_label_set_text(perf_label, "Show FPS/CPU");
	
	lv_obj_t *perf_switch = lv_switch_create(perf_cont, NULL);
	lv_switch_off(perf_switch, LV_ANIM_OFF);  // OFF by default
	lv_obj_set_event_cb(perf_switch, perf_monitor_toggle_cb);
	
	// Add dark theme toggle
	lv_obj_t *theme_cont = lv_cont_create(tab_info, NULL);
	lv_cont_set_layout(theme_cont, LV_LAYOUT_ROW_MID);
	lv_obj_set_width(theme_cont, lv_obj_get_width(tab_info) - 20);
	lv_obj_align(theme_cont, perf_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	lv_obj_t *theme_label = lv_label_create(theme_cont, NULL);
	lv_label_set_text(theme_label, "Dark Theme");
	
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

// Clock update task - called every second
static void clock_update_task(lv_task_t *task)
{
    (void)task;
    
    // Get current time from system (NTP synced)
    time_t now;
    struct tm timeinfo;
    time(&now);
    
    // Apply timezone offset
    now += timezone_offset * 3600;
    
    // Apply winter time adjustment (subtract 1 hour if winter time is enabled)
    // Winter time means UTC+1 (CET), summer time means UTC+2 (CEST)
    if (winter_time_enabled) {
        now -= 3600; // Subtract 1 hour for winter time
    }
    
    localtime_r(&now, &timeinfo);
    
    // Update digital clock display (24-hour format)
    // Note: This is called from LVGL task context, so semaphore is already held
    if (clock_label) {
        char time_str[16];
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        
        // Only update if the text actually changed to avoid unnecessary redraws
        const char *current_text = lv_label_get_text(clock_label);
        if (strcmp(current_text, time_str) != 0) {
            lv_label_set_text(clock_label, time_str);
        }
    }
    
    // Update analog clock hands
    if (analog_clock_gauge != NULL) {
        int hour_12 = timeinfo.tm_hour % 12;  // Convert to 12-hour format
        
        // Add 45 to all values to rotate clock by 270° (puts 12 at top)
        // Gauge coordinate system starts at 90° (6 o'clock), rotate to 0° (12 o'clock)
        const int rotation_offset = 45;  // 3/4 of 60 = 270° rotation
        
        // Second hand: add offset and wrap around 60
        int sec_value = (timeinfo.tm_sec + rotation_offset) % 60;
        lv_gauge_set_value(analog_clock_gauge, 2, sec_value);
        
        // Minute hand: add offset and wrap around 60
        int min_value = (timeinfo.tm_min + rotation_offset) % 60;
        lv_gauge_set_value(analog_clock_gauge, 1, min_value);
        
        // Hour hand: smooth movement with rotation offset
        int hour_value = ((hour_12 * 5) + (timeinfo.tm_min / 12) + rotation_offset) % 60;
        lv_gauge_set_value(analog_clock_gauge, 0, hour_value);
    }
}
