/**
 * @file esp_bsp_sdl_m5_atom_s3.c
 * @brief M5 Atom S3 specific implementation for ESP-BSP SDL abstraction layer
 * Uses official espressif/m5_atom_s3_noglib BSP
 */

#include "sdkconfig.h"

#ifdef CONFIG_ESP_BSP_SDL_BOARD_M5_ATOM_S3

#include "esp_bsp_sdl.h"
#include "esp_log.h"
#include "bsp/m5_atom_s3.h"
#include "bsp/display.h"
#include "esp_lcd_touch.h"

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u

static const char *TAG = "esp_bsp_sdl_m5_atom_s3";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;

esp_err_t esp_bsp_sdl_board_init(esp_bsp_sdl_display_config_t *config, 
                                esp_lcd_panel_handle_t *panel_handle,
                                esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing M5 Atom S3 display using BSP");
    
    if (!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = ESP_OK;
    
    // Step 1: Fill in display configuration for M5 Atom S3 (128x128)
    config->width = 128;  // M5 Atom S3 specific resolution
    config->height = 128; // M5 Atom S3 specific resolution
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (128 * 128) * sizeof(uint16_t);
    config->has_touch = BSP_CAPS_TOUCH == 1;
    
    // Step 2: Initialize backlight PWM control FIRST (M5 Atom S3 requires this)
    ESP_LOGI(TAG, "Initializing backlight control...");
    ret = bsp_display_brightness_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize backlight PWM: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 3: Initialize BSP display using the official BSP
    ESP_LOGI(TAG, "Initializing display panel...");
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = config->max_transfer_sz,
    };
    
    ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 4: Turn on the display
    ESP_LOGI(TAG, "Enabling display...");
    ret = esp_lcd_panel_disp_on_off(s_panel_handle, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 5: Turn on backlight (M5 Atom S3 specific)
    ESP_LOGI(TAG, "Turning on backlight...");
    ret = bsp_display_backlight_on();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on backlight: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;
    
    ESP_LOGI(TAG, "M5 Atom S3 display initialized: %dx%d", config->width, config->height);
    
    return ESP_OK;
}

esp_err_t esp_bsp_sdl_board_backlight_on(void)
{
    ESP_LOGI(TAG, "M5 Atom S3: Turning backlight on");
    return bsp_display_backlight_on();
}

esp_err_t esp_bsp_sdl_board_backlight_off(void)
{
    ESP_LOGI(TAG, "M5 Atom S3: Turning backlight off");
    return bsp_display_backlight_off();
}

esp_err_t esp_bsp_sdl_board_display_on_off(bool enable)
{
    ESP_LOGD(TAG, "%s display", enable ? "Enabling" : "Disabling");
    if (s_panel_handle) {
        return esp_lcd_panel_disp_on_off(s_panel_handle, enable);
    }
    return ESP_ERR_INVALID_STATE;
}

esp_err_t esp_bsp_sdl_board_touch_init(void)
{
#if BSP_CAPS_TOUCH == 1
    ESP_LOGI(TAG, "Initializing touch interface");
    
    // TODO: Implement touch initialization if M5 Atom S3 supports touch
    // For now, just log that touch is theoretically supported but not implemented
    ESP_LOGW(TAG, "Touch support detected in BSP but not yet implemented for M5 Atom S3");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ESP_LOGW(TAG, "Touch not supported on this board configuration");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t esp_bsp_sdl_board_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
#if BSP_CAPS_TOUCH == 1
    if (!touch_info) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // TODO: Implement touch reading if M5 Atom S3 supports touch
    touch_info->pressed = false;
    touch_info->x = 0;
    touch_info->y = 0;
    
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

const char* esp_bsp_sdl_board_get_name(void)
{
    return "M5 Atom S3";
}

esp_err_t esp_bsp_sdl_board_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing M5 Atom S3");
    
    // Clean up resources if needed
    if (s_panel_handle) {
        s_panel_handle = NULL;
    }
    
    if (s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }
    
    return ESP_OK;
}

#endif // CONFIG_ESP_BSP_SDL_BOARD_M5_ATOM_S3
