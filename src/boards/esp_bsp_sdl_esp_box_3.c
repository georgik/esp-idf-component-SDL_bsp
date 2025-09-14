/**
 * @file esp_bsp_sdl_esp_box_3.c
 * @brief ESP-Box-3 specific implementation for ESP-BSP SDL abstraction layer
 */

#include "esp_bsp_sdl.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Include ESP-Box-3 BSP headers
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#if BSP_CAPS_TOUCH == 1
#include "bsp/touch.h"
#endif

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u

static const char *TAG = "esp_bsp_sdl_esp_box_3";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;

esp_err_t esp_bsp_sdl_board_init(esp_bsp_sdl_display_config_t *config, 
                                esp_lcd_panel_handle_t *panel_handle,
                                esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing ESP-Box-3 display");
    
    if (!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Fill in display configuration
    config->width = BSP_LCD_H_RES;
    config->height = BSP_LCD_V_RES;
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (BSP_LCD_H_RES * BSP_LCD_V_RES) * sizeof(uint16_t);
    config->has_touch = BSP_CAPS_TOUCH == 1;
    
    // Initialize BSP display
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = config->max_transfer_sz,
    };
    
    esp_err_t ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;
    
    ESP_LOGI(TAG, "ESP-Box-3 display initialized: %dx%d", config->width, config->height);
    
    return ESP_OK;
}

esp_err_t esp_bsp_sdl_board_backlight_on(void)
{
    ESP_LOGD(TAG, "Turning on backlight");
    return bsp_display_backlight_on();
}

esp_err_t esp_bsp_sdl_board_backlight_off(void)
{
    ESP_LOGD(TAG, "Turning off backlight");
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
    esp_lcd_touch_handle_t touch_handle = NULL;
    esp_err_t ret = bsp_touch_new(NULL, &touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // TODO: Store touch_handle for later use in esp_bsp_sdl_board_touch_read()
    ESP_LOGI(TAG, "Touch interface initialized successfully");
    return ESP_OK;
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
    
    // This is a simplified implementation - in a real implementation,
    // you would store the touch handle and use it here
    touch_info->pressed = false;
    touch_info->x = 0;
    touch_info->y = 0;
    
    // TODO: Implement actual touch reading
    // esp_lcd_touch_read_data(touch_handle);
    
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

const char* esp_bsp_sdl_board_get_name(void)
{
    return "ESP-Box-3";
}

esp_err_t esp_bsp_sdl_board_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing ESP-Box-3");
    
    // Clean up resources if needed
    if (s_panel_handle) {
        // Note: BSP typically handles cleanup, but we could add specific cleanup here
        s_panel_handle = NULL;
    }
    
    if (s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }
    
    return ESP_OK;
}