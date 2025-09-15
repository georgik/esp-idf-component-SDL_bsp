/**
 * @file esp_bsp_sdl_m5stack_core_s3.c
 * @brief M5Stack CoreS3 stub implementation for ESP-BSP SDL abstraction layer
 */

#include "esp_bsp_sdl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "esp_bsp_sdl_m5stack_core_s3";

static esp_err_t m5stack_core_s3_init(esp_bsp_sdl_display_config_t *config,
                                      esp_lcd_panel_handle_t *panel_handle,
                                      esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGE(TAG, "M5Stack Core S3 BSP not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t m5stack_core_s3_backlight_on(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t m5stack_core_s3_backlight_off(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t m5stack_core_s3_display_on_off(bool enable)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t m5stack_core_s3_touch_init(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t m5stack_core_s3_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static const char *m5stack_core_s3_get_name(void)
{
    return "M5Stack Core S3";
}

static esp_err_t m5stack_core_s3_deinit(void)
{
    return ESP_OK;
}

// M5Stack CoreS3 board interface
const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5stack_core_s3_interface = {
    .init = m5stack_core_s3_init,
    .backlight_on = m5stack_core_s3_backlight_on,
    .backlight_off = m5stack_core_s3_backlight_off,
    .display_on_off = m5stack_core_s3_display_on_off,
    .touch_init = m5stack_core_s3_touch_init,
    .touch_read = m5stack_core_s3_touch_read,
    .get_name = m5stack_core_s3_get_name,
    .deinit = m5stack_core_s3_deinit,
    .board_name = "M5Stack CoreS3"};
