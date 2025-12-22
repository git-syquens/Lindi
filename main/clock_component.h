/**
 * @file clock_component.h
 * @brief Reusable analog/digital clock component for LVGL
 * 
 * This component provides a dual-mode clock (analog and digital) with:
 * - Beautiful analog clock with hour, minute, and second hands
 * - Large digital clock display
 * - Toggle button to switch between modes
 * - NVS persistence of user preference
 * - External time management (caller provides time updates)
 */

#ifndef CLOCK_COMPONENT_H
#define CLOCK_COMPONENT_H

#include "lvgl/lvgl.h"
#include <stdbool.h>
#include <time.h>

/**
 * @brief Clock configuration structure
 */
typedef struct {
    lv_obj_t *parent;           ///< Parent container (e.g., tab)
    int x_offset;               ///< X position offset from center
    int y_offset;               ///< Y position offset from center
    bool start_with_digital;    ///< Start in digital mode (vs analog)
    bool show_toggle_button;    ///< Show the digital/analog toggle button
} clock_config_t;

/**
 * @brief Clock handle (opaque pointer)
 */
typedef struct clock_handle_s* clock_handle_t;

/**
 * @brief Create and initialize clock component
 * 
 * Creates both analog and digital clock widgets, with one initially hidden
 * based on configuration. If show_toggle_button is true, creates a toggle
 * button in the top-left corner labeled "dgt clk".
 * 
 * The initial mode is loaded from NVS if available, otherwise uses
 * start_with_digital from config.
 * 
 * @param config Clock configuration
 * @return Clock handle or NULL on error
 * 
 * @note Caller must manage time updates by calling clock_update() periodically
 * @note Uses NVS namespace "lindi_cfg", key "digital_mode"
 */
clock_handle_t clock_create(const clock_config_t *config);

/**
 * @brief Update clock display with current time
 * 
 * Updates both analog and digital clocks (even if hidden) to show the
 * provided time. Call this function at least once per second for smooth
 * second hand movement.
 * 
 * @param handle Clock handle
 * @param timeinfo Pointer to tm structure with current time
 * 
 * @note This function does NOT manage its own LVGL task - the caller
 *       must call this from an LVGL task context or with proper semaphore
 *       locking. See analog_clock.md for integration examples.
 */
void clock_update(clock_handle_t handle, struct tm *timeinfo);

/**
 * @brief Toggle between digital and analog clock modes
 * 
 * Switches visibility between the two clock displays and saves the
 * new preference to NVS for persistence across reboots.
 * 
 * @param handle Clock handle
 */
void clock_toggle_mode(clock_handle_t handle);

/**
 * @brief Get current clock mode
 * 
 * @param handle Clock handle
 * @return true if digital mode, false if analog mode
 */
bool clock_is_digital_mode(clock_handle_t handle);

/**
 * @brief Set clock mode
 * 
 * Programmatically set the clock mode without toggling. Updates both
 * the display and NVS storage.
 * 
 * @param handle Clock handle
 * @param digital true for digital mode, false for analog mode
 */
void clock_set_mode(clock_handle_t handle, bool digital);

/**
 * @brief Destroy clock component and free resources
 * 
 * Deletes all LVGL objects and frees allocated memory. The handle
 * becomes invalid after this call.
 * 
 * @param handle Clock handle
 */
void clock_destroy(clock_handle_t handle);

#endif // CLOCK_COMPONENT_H
