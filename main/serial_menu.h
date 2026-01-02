/**
 * @file serial_menu.h
 * @brief Serial console interactive settings menu
 *
 * Provides UART-based configuration interface for device settings.
 * Allows configuration without touchscreen interaction.
 */

#ifndef SERIAL_MENU_H
#define SERIAL_MENU_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and start the serial menu task
 *
 * Creates a FreeRTOS task that monitors UART for user input
 * and provides an interactive configuration menu.
 *
 * @return ESP_OK on success, ESP_FAIL on error
 */
esp_err_t serial_menu_init(void);

/**
 * @brief Stop the serial menu task
 *
 * Stops the serial menu task if running.
 */
void serial_menu_stop(void);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_MENU_H
