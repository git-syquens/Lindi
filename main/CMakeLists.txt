set(SOURCES main.c clock_component.c)
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES lvgl_esp32_drivers lvgl_touch lvgl_tft lvgl lv_examples esp_event esp_timer esp_wifi nvs_flash driver fatfs sdmmc esp_driver_sdspi)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
