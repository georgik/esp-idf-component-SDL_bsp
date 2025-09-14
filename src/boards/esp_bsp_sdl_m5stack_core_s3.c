/**
 * @file esp_bsp_sdl_m5stack_core_s3.c
 * @brief M5Stack CoreS3 specific implementation for ESP-BSP SDL abstraction layer
 * Uses official espressif/m5stack_core_s3_noglib BSP
 */

#include "esp_bsp_sdl.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "bsp/m5stack_core_s3.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_lcd_touch.h"

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u

static const char *TAG = "esp_bsp_sdl_m5stack_core_s3";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;

esp_err_t esp_bsp_sdl_board_init(esp_bsp_sdl_display_config_t *config, 
                                esp_lcd_panel_handle_t *panel_handle,
                                esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing M5Stack CoreS3 display using BSP");
    
    if (!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = ESP_OK;
    
    // Step 1: Initialize I2C bus (required for PMU and touch controller)
    ESP_LOGI(TAG, "Initializing I2C bus...");
    ret = bsp_i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 2: Initialize display brightness/power management (AXP2101 PMU)
    ESP_LOGI(TAG, "Initializing power management...");
    ret = bsp_display_brightness_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize display brightness: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 3: Fill in display configuration for M5Stack CoreS3
    config->width = BSP_LCD_H_RES;
    config->height = BSP_LCD_V_RES;
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (BSP_LCD_H_RES * BSP_LCD_V_RES) * sizeof(uint16_t);
    config->has_touch = BSP_CAPS_TOUCH == 1;
    
    // Step 4: Initialize BSP display using the official BSP
    ESP_LOGI(TAG, "Initializing display panel...");
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = config->max_transfer_sz,
    };
    
    ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 5: Turn on the display
    ESP_LOGI(TAG, "Enabling display...");
    ret = esp_lcd_panel_disp_on_off(s_panel_handle, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Step 6: Turn on backlight
    ESP_LOGI(TAG, "Turning on backlight...");
    ret = bsp_display_backlight_on();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on backlight: %s", esp_err_to_name(ret));
        // Don't return error here - backlight might work even if this call fails
    }
    
    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;
    
    ESP_LOGI(TAG, "M5Stack CoreS3 display initialized: %dx%d", config->width, config->height);
    
    return ESP_OK;
}

esp_err_t esp_bsp_sdl_board_backlight_on(void)
{
    ESP_LOGI(TAG, "Turning backlight on");
    return bsp_display_backlight_on();
}

int esp_bsp_sdl_board_backlight_off(void)
{
    ESP_LOGI(TAG, "Turning backlight off");
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

static esp_lcd_touch_handle_t s_touch_handle = NULL;

esp_err_t esp_bsp_sdl_board_touch_init(void)
{
#if BSP_CAPS_TOUCH == 1
    ESP_LOGI(TAG, "Initializing touch interface");
    
    // Ensure I2C is initialized (should be done by display init, but check anyway)
    esp_err_t ret = bsp_i2c_init();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize I2C for touch: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = bsp_touch_new(NULL, &s_touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }
    
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
    if (!touch_info || !s_touch_handle) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint16_t touch_x[1] = {0};
    uint16_t touch_y[1] = {0};
    uint16_t touch_strength[1] = {0};
    uint8_t touch_cnt = 0;
    
    // Read touch data
    bool pressed = esp_lcd_touch_read_data(s_touch_handle);
    
    if (pressed) {
        esp_lcd_touch_get_coordinates(s_touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);
        if (touch_cnt > 0) {
            touch_info->pressed = true;
            touch_info->x = touch_x[0];
            touch_info->y = touch_y[0];
        } else {
            touch_info->pressed = false;
            touch_info->x = 0;
            touch_info->y = 0;
        }
    } else {
        touch_info->pressed = false;
        touch_info->x = 0;
        touch_info->y = 0;
    }
    
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

const char* esp_bsp_sdl_board_get_name(void)
{
    return "M5Stack CoreS3";
}

esp_err_t esp_bsp_sdl_board_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing M5Stack CoreS3");
    
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