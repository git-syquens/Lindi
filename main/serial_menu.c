/**
 * @file serial_menu.c
 * @brief Serial console interactive settings menu implementation
 */

#include "serial_menu.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "SerialMenu";

// NVS namespaces
#define NVS_LINDI       "lindi_cfg"
#define NVS_MQTT        "mqtt_cfg"
#define NVS_TIMESRC     "time_cfg"

// External function from main.c to reload calibration offsets
extern void load_calibration_offsets(void);

// Task handle
static TaskHandle_t menu_task_handle = NULL;

// Forward declarations - Main menu
static void serial_menu_task(void *pvParameters);
static void display_main_menu(void);
static void handle_menu_input(char input);
static void print_current_settings(void);

// Forward declarations - Configuration functions
static void configure_wifi(void);
static void configure_timesource(void);
static void configure_mqtt_primary(void);
static void configure_mqtt_secondary(void);
static void configure_timezone(void);
static void configure_language(void);
static void configure_theme(void);
static void configure_level_offsets(void);
static void show_sensor_data(void);
static void factory_reset(void);

// Forward declarations - Helpers
static char read_char_timeout(int timeout_ms);
static void read_line(char *buffer, size_t max_len);
static int read_int(const char *prompt, int min, int max, int default_val);
static bool read_bool(const char *prompt, bool default_val);
static void read_string(const char *prompt, char *buffer, size_t max_len, const char *default_val);
static float read_float(const char *prompt, float min, float max, float default_val);

// NVS helpers
static esp_err_t nvs_get_str_default(const char *ns, const char *key, char *out, size_t *len, const char *def);
static esp_err_t nvs_get_i32_default(const char *ns, const char *key, int32_t *out, int32_t def);
static esp_err_t nvs_get_u8_default(const char *ns, const char *key, uint8_t *out, uint8_t def);
static esp_err_t nvs_get_u16_default(const char *ns, const char *key, uint16_t *out, uint16_t def);
static esp_err_t nvs_set_str_safe(const char *ns, const char *key, const char *value);
static esp_err_t nvs_set_i32_safe(const char *ns, const char *key, int32_t value);
static esp_err_t nvs_set_u8_safe(const char *ns, const char *key, uint8_t value);
static esp_err_t nvs_set_u16_safe(const char *ns, const char *key, uint16_t value);

esp_err_t serial_menu_init(void)
{
    // Uses stdin/stdout from ESP-IDF console (no separate UART init needed)
    // Just create the menu task

    BaseType_t ret = xTaskCreate(
        serial_menu_task,
        "serial_menu",
        4096,
        NULL,
        5,  // Priority
        &menu_task_handle
    );

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create serial menu task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Serial menu initialized. Press 'm' to open menu.");
    return ESP_OK;
}

void serial_menu_stop(void)
{
    if (menu_task_handle != NULL) {
        vTaskDelete(menu_task_handle);
        menu_task_handle = NULL;
    }
}

static void serial_menu_task(void *pvParameters)
{
    bool menu_active = false;

    ESP_LOGI(TAG, "Serial menu task running. Press 'm' for menu.");

    while (1) {
        // Use getchar() instead of uart_read_bytes() to avoid conflicts with ESP-IDF console
        int c = fgetc(stdin);

        if (c != EOF) {
            if (!menu_active && (c == 'm' || c == 'M')) {
                menu_active = true;
                printf("\n");
                display_main_menu();
            } else if (menu_active) {
                if (c == 'q' || c == 'Q' || c == 'x' || c == 'X') {
                    menu_active = false;
                    printf("\n[Menu closed. Press 'm' to reopen]\n\n");
                } else {
                    handle_menu_input(c);
                    if (c != '\r' && c != '\n') {
                        // Redisplay menu after action
                        vTaskDelay(pdMS_TO_TICKS(100));
                        display_main_menu();
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void display_main_menu(void)
{
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║          LINDI - Configuration Menu v2.0              ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  GENERAL\n");
    printf("  [1] View Current Settings\n");
    printf("  [2] Configure WiFi (requires rebuild)\n");
    printf("  [3] Set Timezone\n");
    printf("  [4] Change Language (EN/NL)\n");
    printf("  [5] Toggle Theme (Light/Dark)\n");
    printf("\n");
    printf("  TIME SOURCE\n");
    printf("  [6] Configure Time Source (WiFi/NTP)\n");
    printf("\n");
    printf("  MQTT CONFIGURATION\n");
    printf("  [7] Configure Primary MQTT Server\n");
    printf("  [8] Configure Secondary MQTT Server\n");
    printf("\n");
    printf("  LEVEL CALIBRATION\n");
    printf("  [9] Configure Level Offsets\n");
    printf("  [s] Show Live Sensor Data\n");
    printf("\n");
    printf("  SYSTEM\n");
    printf("  [f] Factory Reset\n");
    printf("  [r] Reboot Device\n");
    printf("  [q] Exit Menu\n");
    printf("\n");
    printf("Select option: ");
    fflush(stdout);
}

static void handle_menu_input(char input)
{
    printf("%c\n\n", input);  // Echo input

    switch (input) {
        case '1':
            print_current_settings();
            break;
        case '2':
            configure_wifi();
            break;
        case '3':
            configure_timezone();
            break;
        case '4':
            configure_language();
            break;
        case '5':
            configure_theme();
            break;
        case '6':
            configure_timesource();
            break;
        case '7':
            configure_mqtt_primary();
            break;
        case '8':
            configure_mqtt_secondary();
            break;
        case '9':
            configure_level_offsets();
            break;
        case 's':
        case 'S':
            show_sensor_data();
            break;
        case 'f':
        case 'F':
            factory_reset();
            break;
        case 'r':
        case 'R':
            printf("Rebooting device in 2 seconds...\n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            esp_restart();
            break;
        case 'q':
        case 'Q':
        case 'x':
        case 'X':
            // Handled in main loop
            break;
        default:
            if (input != '\r' && input != '\n') {
                printf("Invalid option. Please try again.\n");
            }
            break;
    }
}

static void print_current_settings(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  CURRENT SETTINGS\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    // === GENERAL SETTINGS ===
    printf("GENERAL:\n");

    // Timezone
    int32_t timezone = 0;
    nvs_get_i32_default(NVS_LINDI, "timezone", &timezone, 1);
    printf("  Timezone:     GMT%+ld\n", (long)timezone);

    // Winter time
    uint8_t winter_time = 0;
    nvs_get_u8_default(NVS_LINDI, "winter_time", &winter_time, 0);
    printf("  Winter Time:  %s\n", winter_time ? "Enabled" : "Disabled");

    // Language
    uint8_t lang = 0;
    nvs_get_u8_default(NVS_LINDI, "language", &lang, 0);
    printf("  Language:     %s\n", lang == 0 ? "English" : "Nederlands");

    // Theme
    uint8_t dark_theme = 0;
    nvs_get_u8_default(NVS_LINDI, "dark_theme", &dark_theme, 1);
    printf("  Theme:        %s\n", dark_theme ? "Dark" : "Light");

    // Clock mode
    uint8_t digital_mode = 0;
    nvs_get_u8_default(NVS_LINDI, "digital_mode", &digital_mode, 0);
    printf("  Clock Mode:   %s\n", digital_mode ? "Digital" : "Analog");

    printf("\n");

    // === TIME SOURCE ===
    printf("TIME SOURCE:\n");

    uint8_t use_ntp = 1;
    nvs_get_u8_default(NVS_TIMESRC, "use_ntp", &use_ntp, 1);
    printf("  Source:       %s\n", use_ntp ? "WiFi/NTP" : "Manual NTP Server");

    if (!use_ntp) {
        char ntp_server[64] = {0};
        size_t len = sizeof(ntp_server);
        nvs_get_str_default(NVS_TIMESRC, "ntp_server", ntp_server, &len, "pool.ntp.org");
        printf("  NTP Server:   %s\n", ntp_server);
    }

    printf("\n");

    // === MQTT PRIMARY ===
    printf("MQTT PRIMARY SERVER:\n");

    char mqtt1_addr[128] = {0};
    size_t len = sizeof(mqtt1_addr);
    nvs_get_str_default(NVS_MQTT, "mqtt1_addr", mqtt1_addr, &len, "mqtt.syquens.com");
    printf("  Address:      %s\n", mqtt1_addr);

    uint16_t mqtt1_port = 8883;
    nvs_get_u16_default(NVS_MQTT, "mqtt1_port", &mqtt1_port, 8883);
    printf("  Port:         %u\n", mqtt1_port);

    uint8_t mqtt1_tls = 1;
    nvs_get_u8_default(NVS_MQTT, "mqtt1_tls", &mqtt1_tls, 1);
    printf("  TLS:          %s\n", mqtt1_tls ? "Enabled" : "Disabled");

    char mqtt1_user[64] = {0};
    len = sizeof(mqtt1_user);
    nvs_get_str_default(NVS_MQTT, "mqtt1_user", mqtt1_user, &len, "23-09edh");
    printf("  Username:     %s\n", mqtt1_user);

    printf("  Password:     %s\n", strlen(mqtt1_user) > 0 ? "********" : "(not set)");

    printf("\n");

    // === MQTT SECONDARY ===
    printf("MQTT SECONDARY SERVER:\n");

    char mqtt2_addr[128] = {0};
    len = sizeof(mqtt2_addr);
    nvs_get_str_default(NVS_MQTT, "mqtt2_addr", mqtt2_addr, &len, "");
    printf("  Address:      %s\n", strlen(mqtt2_addr) > 0 ? mqtt2_addr : "(not configured)");

    if (strlen(mqtt2_addr) > 0) {
        uint16_t mqtt2_port = 1883;
        nvs_get_u16_default(NVS_MQTT, "mqtt2_port", &mqtt2_port, 1883);
        printf("  Port:         %u\n", mqtt2_port);

        uint8_t mqtt2_tls = 0;
        nvs_get_u8_default(NVS_MQTT, "mqtt2_tls", &mqtt2_tls, 0);
        printf("  TLS:          %s\n", mqtt2_tls ? "Enabled" : "Disabled");

        char mqtt2_user[64] = {0};
        len = sizeof(mqtt2_user);
        nvs_get_str_default(NVS_MQTT, "mqtt2_user", mqtt2_user, &len, "");
        printf("  Username:     %s\n", strlen(mqtt2_user) > 0 ? mqtt2_user : "(none)");

        printf("  Password:     %s\n", strlen(mqtt2_user) > 0 ? "********" : "(none)");
    }

    printf("\n");

    // === LEVEL OFFSETS ===
    printf("LEVEL CALIBRATION OFFSETS:\n");

    // Read offsets (stored as int32 in millidegrees)
    int32_t pitch_offset_millideg = 0;
    int32_t roll_offset_millideg = 0;
    nvs_get_i32_default(NVS_LINDI, "pitch_off", &pitch_offset_millideg, 0);
    nvs_get_i32_default(NVS_LINDI, "roll_off", &roll_offset_millideg, 0);

    float pitch_offset = pitch_offset_millideg / 1000.0f;
    float roll_offset = roll_offset_millideg / 1000.0f;

    printf("  Pitch Offset: %+.3f°\n", pitch_offset);
    printf("  Roll Offset:  %+.3f°\n", roll_offset);

    uint8_t invert_level = 0;
    nvs_get_u8_default(NVS_LINDI, "invert_level", &invert_level, 0);
    printf("  Invert Level: %s\n", invert_level ? "Yes" : "No");

    printf("\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\nPress any key to return to menu...");
    fflush(stdout);
    read_char_timeout(10000);
    printf("\n");
}

static void configure_wifi(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  WiFi Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("WiFi credentials are compiled into firmware.\n");
    printf("\n");
    printf("To change WiFi settings:\n");
    printf("  1. Edit main/wifi_credentials.h\n");
    printf("  2. Set WIFI_SSID and WIFI_PASS\n");
    printf("  3. Rebuild and reflash firmware\n");
    printf("\n");
    printf("Press any key to continue...");
    fflush(stdout);
    read_char_timeout(10000);
    printf("\n");
}

static void configure_timesource(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Time Source Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    uint8_t use_ntp = 1;
    nvs_get_u8_default(NVS_TIMESRC, "use_ntp", &use_ntp, 1);

    printf("Current setting: %s\n", use_ntp ? "WiFi/Auto NTP" : "Manual NTP Server");
    printf("\n");
    printf("Select time source:\n");
    printf("  [1] WiFi/Auto NTP (pool.ntp.org, time.google.com)\n");
    printf("  [2] Manual NTP Server IP\n");
    printf("\n");

    int choice = read_int("Choice", 1, 2, use_ntp ? 1 : 2);

    if (choice == 1) {
        nvs_set_u8_safe(NVS_TIMESRC, "use_ntp", 1);
        printf("✓ Time source set to WiFi/Auto NTP\n");
        printf("  Using: pool.ntp.org, time.google.com\n");
    } else {
        char ntp_server[64] = {0};
        size_t len = sizeof(ntp_server);
        nvs_get_str_default(NVS_TIMESRC, "ntp_server", ntp_server, &len, "pool.ntp.org");

        printf("\nCurrent NTP server: %s\n", ntp_server);
        read_string("Enter NTP server IP/hostname", ntp_server, sizeof(ntp_server), ntp_server);

        nvs_set_u8_safe(NVS_TIMESRC, "use_ntp", 0);
        nvs_set_str_safe(NVS_TIMESRC, "ntp_server", ntp_server);

        printf("✓ NTP server set to: %s\n", ntp_server);
    }

    printf("  Restart device to apply changes.\n");
    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_mqtt_primary(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Primary MQTT Server Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    char addr[128], user[64], pass[128];
    uint16_t port;
    uint8_t tls;

    // Load current settings
    size_t len = sizeof(addr);
    nvs_get_str_default(NVS_MQTT, "mqtt1_addr", addr, &len, "mqtt.syquens.com");
    nvs_get_u16_default(NVS_MQTT, "mqtt1_port", &port, 8883);
    nvs_get_u8_default(NVS_MQTT, "mqtt1_tls", &tls, 1);
    len = sizeof(user);
    nvs_get_str_default(NVS_MQTT, "mqtt1_user", user, &len, "23-09edh");
    len = sizeof(pass);
    nvs_get_str_default(NVS_MQTT, "mqtt1_pass", pass, &len, "");

    // Configure
    printf("Current settings:\n");
    printf("  Address: %s\n", addr);
    printf("  Port:    %u\n", port);
    printf("  TLS:     %s\n", tls ? "Enabled" : "Disabled");
    printf("  User:    %s\n", user);
    printf("\n");

    read_string("Server address", addr, sizeof(addr), addr);
    port = read_int("Port", 1, 65535, port);
    tls = read_bool("Enable TLS", tls);
    read_string("Username (leave empty for none)", user, sizeof(user), user);

    if (strlen(user) > 0) {
        read_string("Password", pass, sizeof(pass), "");
    } else {
        pass[0] = '\0';
    }

    // Save
    nvs_set_str_safe(NVS_MQTT, "mqtt1_addr", addr);
    nvs_set_u16_safe(NVS_MQTT, "mqtt1_port", port);
    nvs_set_u8_safe(NVS_MQTT, "mqtt1_tls", tls ? 1 : 0);
    nvs_set_str_safe(NVS_MQTT, "mqtt1_user", user);
    nvs_set_str_safe(NVS_MQTT, "mqtt1_pass", pass);

    printf("\n✓ Primary MQTT server configured\n");
    printf("  %s://%s:%u\n", tls ? "mqtts" : "mqtt", addr, port);
    printf("  Restart device to apply changes.\n");

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_mqtt_secondary(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Secondary MQTT Server Configuration (Failover)\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    char addr[128], user[64], pass[128];
    uint16_t port;
    uint8_t tls;

    // Load current settings
    size_t len = sizeof(addr);
    nvs_get_str_default(NVS_MQTT, "mqtt2_addr", addr, &len, "");
    nvs_get_u16_default(NVS_MQTT, "mqtt2_port", &port, 1883);
    nvs_get_u8_default(NVS_MQTT, "mqtt2_tls", &tls, 0);
    len = sizeof(user);
    nvs_get_str_default(NVS_MQTT, "mqtt2_user", user, &len, "");
    len = sizeof(pass);
    nvs_get_str_default(NVS_MQTT, "mqtt2_pass", pass, &len, "");

    // Configure
    if (strlen(addr) > 0) {
        printf("Current settings:\n");
        printf("  Address: %s\n", addr);
        printf("  Port:    %u\n", port);
        printf("  TLS:     %s\n", tls ? "Enabled" : "Disabled");
        printf("  User:    %s\n", user);
        printf("\n");
    } else {
        printf("Secondary server not configured.\n\n");
    }

    printf("Configure secondary server? (leave address empty to disable)\n\n");

    read_string("Server address (empty to disable)", addr, sizeof(addr), addr);

    if (strlen(addr) == 0) {
        // Disable secondary server
        nvs_set_str_safe(NVS_MQTT, "mqtt2_addr", "");
        printf("\n✓ Secondary MQTT server disabled\n");
    } else {
        port = read_int("Port", 1, 65535, port);
        tls = read_bool("Enable TLS", tls);
        read_string("Username (leave empty for none)", user, sizeof(user), user);

        if (strlen(user) > 0) {
            read_string("Password", pass, sizeof(pass), "");
        } else {
            pass[0] = '\0';
        }

        // Save
        nvs_set_str_safe(NVS_MQTT, "mqtt2_addr", addr);
        nvs_set_u16_safe(NVS_MQTT, "mqtt2_port", port);
        nvs_set_u8_safe(NVS_MQTT, "mqtt2_tls", tls ? 1 : 0);
        nvs_set_str_safe(NVS_MQTT, "mqtt2_user", user);
        nvs_set_str_safe(NVS_MQTT, "mqtt2_pass", pass);

        printf("\n✓ Secondary MQTT server configured\n");
        printf("  %s://%s:%u\n", tls ? "mqtts" : "mqtt", addr, port);
    }

    printf("  Restart device to apply changes.\n");
    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_timezone(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Timezone Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    int32_t current_tz = 0;
    nvs_get_i32_default(NVS_LINDI, "timezone", &current_tz, 1);
    printf("Current timezone: GMT%+ld\n", (long)current_tz);
    printf("\n");

    int new_tz = read_int("Enter new timezone", -12, 12, current_tz);

    nvs_set_i32_safe(NVS_LINDI, "timezone", new_tz);
    printf("\n✓ Timezone set to GMT%+d\n", new_tz);  // new_tz is int, so %d is correct
    printf("  Restart device to apply changes.\n");

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_language(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Language Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    uint8_t current_lang = 0;
    nvs_get_u8_default(NVS_LINDI, "language", &current_lang, 0);
    printf("Current language: %s\n", current_lang == 0 ? "English" : "Nederlands");
    printf("\n");
    printf("Select language:\n");
    printf("  [0] English\n");
    printf("  [1] Nederlands\n");
    printf("\n");

    int choice = read_int("Choice", 0, 1, current_lang);

    nvs_set_u8_safe(NVS_LINDI, "language", (uint8_t)choice);
    printf("\n✓ Language set to %s\n", choice == 0 ? "English" : "Nederlands");
    printf("  Restart device to update tab names.\n");

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_theme(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Theme Configuration\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    uint8_t current_theme = 1;
    nvs_get_u8_default(NVS_LINDI, "dark_theme", &current_theme, 1);
    printf("Current theme: %s\n", current_theme ? "Dark" : "Light");
    printf("\n");

    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Toggle to %s theme", current_theme ? "Light" : "Dark");
    bool toggle = read_bool(prompt, false);

    if (toggle) {
        uint8_t new_theme = !current_theme;
        nvs_set_u8_safe(NVS_LINDI, "dark_theme", new_theme);
        printf("\n✓ Theme toggled to %s\n", new_theme ? "Dark" : "Light");
        printf("  Changes apply immediately on GUI.\n");
    } else {
        printf("\nTheme unchanged.\n");
    }

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void configure_level_offsets(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  Level Calibration Offsets\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");

    // Read current offsets
    int32_t pitch_millideg = 0, roll_millideg = 0;
    nvs_get_i32_default(NVS_LINDI, "pitch_off", &pitch_millideg, 0);
    nvs_get_i32_default(NVS_LINDI, "roll_off", &roll_millideg, 0);

    float pitch_offset = pitch_millideg / 1000.0f;
    float roll_offset = roll_millideg / 1000.0f;

    printf("Current offsets:\n");
    printf("  Pitch: %+.3f°\n", pitch_offset);
    printf("  Roll:  %+.3f°\n", roll_offset);
    printf("\n");
    printf("Options:\n");
    printf("  [1] Auto-calibrate (place level, read current position)\n");
    printf("  [2] Manually set pitch offset\n");
    printf("  [3] Manually set roll offset\n");
    printf("  [4] Reset offsets to zero\n");
    printf("  [0] Cancel\n");
    printf("\n");

    int choice = read_int("Choice", 0, 4, 0);

    switch (choice) {
        case 1:
            printf("\n⚠️  AUTO-CALIBRATION\n");
            printf("1. Place device on PERFECTLY level surface\n");
            printf("2. Wait 2 seconds for sensor to stabilize\n");
            printf("3. Current reading will be saved as zero reference\n");
            printf("\n");

            if (read_bool("Proceed with auto-calibration", false)) {
                printf("\nCalibrating in 2 seconds...\n");
                vTaskDelay(pdMS_TO_TICKS(2000));
                printf("Reading sensor... (TODO: integrate with MPU6050)\n");
                printf("✓ Calibration complete!\n");
                printf("  (Feature pending MPU6050 integration)\n");
            } else {
                printf("\nCalibration cancelled.\n");
            }
            break;

        case 2:
            printf("\n");
            pitch_offset = read_float("Enter pitch offset (degrees)", -30.0f, 30.0f, pitch_offset);
            pitch_millideg = (int32_t)(pitch_offset * 1000.0f);
            nvs_set_i32_safe(NVS_LINDI, "pitch_off", pitch_millideg);
            load_calibration_offsets();  // Reload offsets into main.c static variables
            printf("✓ Pitch offset set to %+.3f°\n", pitch_offset);
            printf("  Changes applied immediately.\n");
            break;

        case 3:
            printf("\n");
            roll_offset = read_float("Enter roll offset (degrees)", -30.0f, 30.0f, roll_offset);
            roll_millideg = (int32_t)(roll_offset * 1000.0f);
            nvs_set_i32_safe(NVS_LINDI, "roll_off", roll_millideg);
            load_calibration_offsets();  // Reload offsets into main.c static variables
            printf("✓ Roll offset set to %+.3f°\n", roll_offset);
            printf("  Changes applied immediately.\n");
            break;

        case 4:
            if (read_bool("Reset offsets to zero", false)) {
                nvs_set_i32_safe(NVS_LINDI, "pitch_off", 0);
                nvs_set_i32_safe(NVS_LINDI, "roll_off", 0);
                load_calibration_offsets();  // Reload offsets into main.c static variables
                printf("\n✓ Offsets reset to zero\n");
                printf("  Changes applied immediately.\n");
            } else {
                printf("\nReset cancelled.\n");
            }
            break;

        default:
            printf("\nCancelled.\n");
            break;
    }

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void show_sensor_data(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  MPU6050 Live Sensor Data\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("Displaying live sensor data for 10 seconds...\n");
    printf("Press any key to return to menu.\n");
    printf("\n");
    printf("  Pitch    Roll     Temp\n");
    printf("  ------   ------   ------\n");

    // TODO: Integrate with actual MPU6050 reading
    // For now, show placeholder
    for (int i = 0; i < 10; i++) {
        printf("\r  %+6.2f°  %+6.2f°  %.1f°C   ",
               0.0f, 0.0f, 25.0f);  // Placeholder values
        fflush(stdout);

        // Check for keypress (use read_char_timeout with 1s timeout)
        char c = read_char_timeout(1000);
        if (c != 0) {
            break;
        }
    }

    printf("\n\n(Feature pending MPU6050 integration)\n");
    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

static void factory_reset(void)
{
    printf("════════════════════════════════════════════════════════\n");
    printf("  FACTORY RESET\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("⚠️  WARNING: This will erase ALL settings!\n");
    printf("\n");
    printf("Settings that will be erased:\n");
    printf("  • Timezone, language, theme preferences\n");
    printf("  • Time source configuration\n");
    printf("  • MQTT server settings\n");
    printf("  • Level calibration offsets\n");
    printf("  • All user settings\n");
    printf("\n");
    printf("Settings that will be PRESERVED:\n");
    printf("  • WiFi credentials (compiled in firmware)\n");
    printf("\n");
    printf("Type 'RESET' to confirm: ");
    fflush(stdout);

    char buf[32];
    read_line(buf, sizeof(buf));

    if (strcmp(buf, "RESET") == 0) {
        printf("\nErasing NVS...\n");
        esp_err_t err = nvs_flash_erase();
        if (err == ESP_OK) {
            printf("✓ Factory reset complete!\n");
            printf("  Rebooting in 3 seconds...\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
            esp_restart();
        } else {
            printf("✗ Failed to erase NVS: %s\n", esp_err_to_name(err));
        }
    } else {
        printf("\nFactory reset cancelled.\n");
    }

    printf("\nPress any key to continue...");
    fflush(stdout);
    read_char_timeout(3000);
    printf("\n");
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static char read_char_timeout(int timeout_ms)
{
    // Use fgetc with a simple timeout loop instead of uart_read_bytes
    TickType_t start = xTaskGetTickCount();
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);

    while ((xTaskGetTickCount() - start) < timeout_ticks) {
        int c = fgetc(stdin);
        if (c != EOF) {
            return (char)c;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;  // Timeout
}

static void read_line(char *buffer, size_t max_len)
{
    size_t pos = 0;
    while (pos < max_len - 1) {
        int c = fgetc(stdin);
        if (c != EOF) {
            if (c == '\n' || c == '\r') {
                break;
            }
            if (c == 8 || c == 127) {  // Backspace
                if (pos > 0) {
                    pos--;
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (c >= 32 && c < 127) {  // Printable characters
                buffer[pos++] = c;
                printf("%c", c);
                fflush(stdout);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));  // Avoid tight loop when no data
        }
    }
    buffer[pos] = '\0';
    printf("\n");
}

static int read_int(const char *prompt, int min, int max, int default_val)
{
    char buf[32];
    printf("%s [%d-%d] (default: %d): ", prompt, min, max, default_val);
    fflush(stdout);

    read_line(buf, sizeof(buf));

    if (strlen(buf) == 0) {
        return default_val;
    }

    int val = atoi(buf);
    if (val < min || val > max) {
        printf("⚠️  Value out of range, using default: %d\n", default_val);
        return default_val;
    }

    return val;
}

static bool read_bool(const char *prompt, bool default_val)
{
    printf("%s (y/n) [default: %c]: ", prompt, default_val ? 'y' : 'n');
    fflush(stdout);

    char c = read_char_timeout(10000);
    printf("%c\n", c);

    if (c == 'y' || c == 'Y') return true;
    if (c == 'n' || c == 'N') return false;

    return default_val;
}

static void read_string(const char *prompt, char *buffer, size_t max_len, const char *default_val)
{
    printf("%s", prompt);
    if (default_val && strlen(default_val) > 0) {
        printf(" [default: %s]", default_val);
    }
    printf(": ");
    fflush(stdout);

    read_line(buffer, max_len);

    if (strlen(buffer) == 0 && default_val) {
        strncpy(buffer, default_val, max_len - 1);
        buffer[max_len - 1] = '\0';
    }
}

static float read_float(const char *prompt, float min, float max, float default_val)
{
    char buf[32];
    printf("%s [%.2f-%.2f] (default: %.3f): ", prompt, min, max, default_val);
    fflush(stdout);

    read_line(buf, sizeof(buf));

    if (strlen(buf) == 0) {
        return default_val;
    }

    float val = atof(buf);
    if (val < min || val > max) {
        printf("⚠️  Value out of range, using default: %.3f\n", default_val);
        return default_val;
    }

    return val;
}

// ============================================================================
// NVS HELPER FUNCTIONS
// ============================================================================

static esp_err_t nvs_get_str_default(const char *ns, const char *key, char *out, size_t *len, const char *def)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        if (def) strncpy(out, def, *len);
        return err;
    }

    err = nvs_get_str(handle, key, out, len);
    nvs_close(handle);

    if (err != ESP_OK && def) {
        strncpy(out, def, *len);
    }
    return err;
}

static esp_err_t nvs_get_i32_default(const char *ns, const char *key, int32_t *out, int32_t def)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        *out = def;
        return err;
    }

    err = nvs_get_i32(handle, key, out);
    nvs_close(handle);

    if (err != ESP_OK) {
        *out = def;
    }
    return err;
}

static esp_err_t nvs_get_u8_default(const char *ns, const char *key, uint8_t *out, uint8_t def)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        *out = def;
        return err;
    }

    err = nvs_get_u8(handle, key, out);
    nvs_close(handle);

    if (err != ESP_OK) {
        *out = def;
    }
    return err;
}

static esp_err_t nvs_get_u16_default(const char *ns, const char *key, uint16_t *out, uint16_t def)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        *out = def;
        return err;
    }

    err = nvs_get_u16(handle, key, out);
    nvs_close(handle);

    if (err != ESP_OK) {
        *out = def;
    }
    return err;
}

static esp_err_t nvs_set_str_safe(const char *ns, const char *key, const char *value)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_str(handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_set_i32_safe(const char *ns, const char *key, int32_t value)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_i32(handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_set_u8_safe(const char *ns, const char *key, uint8_t value)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_u8(handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

static esp_err_t nvs_set_u16_safe(const char *ns, const char *key, uint16_t value)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(ns, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_u16(handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}
