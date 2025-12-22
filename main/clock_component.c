/**
 * @file clock_component.c
 * @brief Implementation of reusable analog/digital clock component
 */

#include "clock_component.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>

#define NVS_NAMESPACE "lindi_cfg"
#define NVS_KEY_DIGITAL_MODE "digital_mode"

static const char *TAG = "clock_component";

/**
 * @brief Internal clock component structure
 */
struct clock_handle_s {
    lv_obj_t *analog_gauge;      ///< Analog clock gauge widget
    lv_obj_t *digital_label;     ///< Digital clock label widget
    lv_obj_t *toggle_button;     ///< Mode toggle button (may be NULL)
    bool digital_mode;           ///< Current mode: true=digital, false=analog
    int x_offset;                ///< X position offset
    int y_offset;                ///< Y position offset
};

// Forward declarations
static void load_digital_clock_mode(clock_handle_t handle);
static void save_digital_clock_mode(bool enabled);
static void toggle_button_cb(lv_obj_t *obj, lv_event_t event);

/**
 * @brief Load digital clock mode setting from NVS
 */
static void load_digital_clock_mode(clock_handle_t handle)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t value = 0;
        err = nvs_get_u8(nvs_handle, NVS_KEY_DIGITAL_MODE, &value);
        if (err == ESP_OK) {
            handle->digital_mode = (value != 0);
            ESP_LOGI(TAG, "Loaded digital clock mode: %s", handle->digital_mode ? "enabled" : "disabled");
        } else {
            ESP_LOGI(TAG, "Digital clock mode not found in NVS, using config default");
        }
        nvs_close(nvs_handle);
    }
}

/**
 * @brief Save digital clock mode setting to NVS
 */
static void save_digital_clock_mode(bool enabled)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_set_u8(nvs_handle, NVS_KEY_DIGITAL_MODE, enabled ? 1 : 0);
        if (err == ESP_OK) {
            err = nvs_commit(nvs_handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Saved digital clock mode: %s", enabled ? "enabled" : "disabled");
            }
        }
        nvs_close(nvs_handle);
    }
}

/**
 * @brief Toggle button event callback
 */
static void toggle_button_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        // Get handle from user_data
        clock_handle_t handle = (clock_handle_t)lv_obj_get_user_data(obj);
        if (handle) {
            clock_toggle_mode(handle);
        }
    }
}

/**
 * @brief Create and initialize clock component
 */
clock_handle_t clock_create(const clock_config_t *config)
{
    if (!config || !config->parent) {
        ESP_LOGE(TAG, "Invalid config");
        return NULL;
    }

    // Allocate handle
    clock_handle_t handle = (clock_handle_t)malloc(sizeof(struct clock_handle_s));
    if (!handle) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return NULL;
    }

    // Initialize handle
    memset(handle, 0, sizeof(struct clock_handle_s));
    handle->digital_mode = config->start_with_digital;
    handle->x_offset = config->x_offset;
    handle->y_offset = config->y_offset;

    // Load saved mode from NVS (may override config default)
    load_digital_clock_mode(handle);

    // Create toggle button if requested
    if (config->show_toggle_button) {
        handle->toggle_button = lv_btn_create(config->parent, NULL);
        lv_obj_set_size(handle->toggle_button, 40, 30);
        lv_obj_align(handle->toggle_button, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 5);
        lv_obj_set_user_data(handle->toggle_button, handle);
        lv_obj_set_event_cb(handle->toggle_button, toggle_button_cb);

        // Add label to the right of button
        lv_obj_t *btn_label = lv_label_create(config->parent, NULL);
        lv_label_set_text(btn_label, "dgt clk");
        lv_obj_set_style_local_text_font(btn_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
        lv_obj_align(btn_label, handle->toggle_button, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    }

    // Create digital clock label
    handle->digital_label = lv_label_create(config->parent, NULL);
    lv_label_set_text(handle->digital_label, "--:--:--");
    lv_obj_set_style_local_text_font(handle->digital_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_obj_align(handle->digital_label, NULL, LV_ALIGN_CENTER, config->x_offset, config->y_offset);

    // Create analog clock gauge
    handle->analog_gauge = lv_gauge_create(config->parent, NULL);
    lv_obj_set_size(handle->analog_gauge, 139, 139);  // 110% of base 126
    lv_obj_align(handle->analog_gauge, NULL, LV_ALIGN_CENTER, config->x_offset, config->y_offset + 10);

    // Configure gauge for clock: 360° full circle with 60 tick marks
    lv_gauge_set_scale(handle->analog_gauge, 360, 60, 0);  // 360° arc, 60 tick lines, 0 labels
    lv_gauge_set_range(handle->analog_gauge, 0, 59);       // 0-59 range (each unit = 6°)
    lv_gauge_set_angle_offset(handle->analog_gauge, 270);  // Rotate so value 0 is at bottom

    // Set up 3 needles: hour (light gray), minute (darker gray), second (red)
    static lv_color_t needle_colors[3];
    needle_colors[0] = LV_COLOR_MAKE(200, 200, 200);  // Hour hand
    needle_colors[1] = LV_COLOR_MAKE(150, 150, 150);  // Minute hand
    needle_colors[2] = LV_COLOR_RED;                   // Second hand
    lv_gauge_set_needle_count(handle->analog_gauge, 3, needle_colors);

    // Make needles 1.5x longer by reducing inner padding
    lv_obj_set_style_local_pad_inner(handle->analog_gauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 10);

    // Set initial needle positions to 12:00:00
    lv_gauge_set_value(handle->analog_gauge, 0, 30);  // Hour at 12 (with offset)
    lv_gauge_set_value(handle->analog_gauge, 1, 30);  // Minute at 12 (with offset)
    lv_gauge_set_value(handle->analog_gauge, 2, 30);  // Second at 12 (with offset)

    // Apply initial visibility based on loaded mode
    lv_obj_set_hidden(handle->digital_label, !handle->digital_mode);
    lv_obj_set_hidden(handle->analog_gauge, handle->digital_mode);

    ESP_LOGI(TAG, "Clock created in %s mode", handle->digital_mode ? "digital" : "analog");
    return handle;
}

/**
 * @brief Update clock display with current time
 */
void clock_update(clock_handle_t handle, struct tm *timeinfo)
{
    if (!handle || !timeinfo) {
        return;
    }

    // Update digital clock display (24-hour format)
    if (handle->digital_label) {
        char time_str[16];
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d",
                 timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

        // Only update if text changed to avoid unnecessary redraws
        const char *current_text = lv_label_get_text(handle->digital_label);
        if (strcmp(current_text, time_str) != 0) {
            lv_label_set_text(handle->digital_label, time_str);
        }
    }

    // Update analog clock hands
    if (handle->analog_gauge) {
        int hour_12 = timeinfo->tm_hour % 12;  // Convert to 12-hour format

        // Rotation offset for 360° arc with angle_offset=270°
        // angle_offset=270° puts value 0 at 6 o'clock (bottom)
        // Need +30 units to reach 12 o'clock (top): 180° / 6° = 30
        const int rotation_offset = 30;

        // Second hand: direct mapping (0-59 seconds)
        int sec_value = (timeinfo->tm_sec + rotation_offset) % 60;
        lv_gauge_set_value(handle->analog_gauge, 2, sec_value);

        // Minute hand: direct mapping (0-59 minutes)
        int min_value = (timeinfo->tm_min + rotation_offset) % 60;
        lv_gauge_set_value(handle->analog_gauge, 1, min_value);

        // Hour hand: 5 units per hour + smooth interpolation based on minutes
        // Each hour = 5 units (60 units total / 12 hours = 5 units/hour)
        int hour_value = ((hour_12 * 5) + (timeinfo->tm_min * 5 / 60) + rotation_offset) % 60;
        lv_gauge_set_value(handle->analog_gauge, 0, hour_value);
    }
}

/**
 * @brief Toggle between digital and analog clock modes
 */
void clock_toggle_mode(clock_handle_t handle)
{
    if (!handle) {
        return;
    }

    // Toggle mode
    handle->digital_mode = !handle->digital_mode;

    // Save to NVS
    save_digital_clock_mode(handle->digital_mode);

    // Update visibility
    if (handle->digital_mode) {
        lv_obj_set_hidden(handle->digital_label, false);
        lv_obj_set_hidden(handle->analog_gauge, true);
    } else {
        lv_obj_set_hidden(handle->digital_label, true);
        lv_obj_set_hidden(handle->analog_gauge, false);
    }

    ESP_LOGI(TAG, "Clock mode changed to %s", handle->digital_mode ? "digital" : "analog");
}

/**
 * @brief Get current clock mode
 */
bool clock_is_digital_mode(clock_handle_t handle)
{
    return handle ? handle->digital_mode : false;
}

/**
 * @brief Set clock mode
 */
void clock_set_mode(clock_handle_t handle, bool digital)
{
    if (!handle || handle->digital_mode == digital) {
        return;  // Already in requested mode
    }

    clock_toggle_mode(handle);
}

/**
 * @brief Destroy clock component and free resources
 */
void clock_destroy(clock_handle_t handle)
{
    if (!handle) {
        return;
    }

    // Delete LVGL objects (automatically handles NULL)
    if (handle->analog_gauge) {
        lv_obj_del(handle->analog_gauge);
    }
    if (handle->digital_label) {
        lv_obj_del(handle->digital_label);
    }
    if (handle->toggle_button) {
        lv_obj_del(handle->toggle_button);
    }

    // Free handle memory
    free(handle);

    ESP_LOGI(TAG, "Clock component destroyed");
}
