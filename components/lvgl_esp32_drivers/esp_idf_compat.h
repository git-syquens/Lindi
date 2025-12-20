#ifndef ESP_IDF_COMPAT_H
#define ESP_IDF_COMPAT_H

// Compatibility layer for ESP-IDF 5.x

#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

// GPIO function renamed in ESP-IDF 5.x
#define gpio_pad_select_gpio(gpio_num) esp_rom_gpio_pad_select_gpio(gpio_num)

// Delay function renamed in ESP-IDF 5.x
#define ets_delay_us(us) esp_rom_delay_us(us)

// FreeRTOS constant renamed in ESP-IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#endif // ESP_IDF_COMPAT_H
