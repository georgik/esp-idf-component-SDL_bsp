/**
 * @file esp_bsp_sdl_common.c
 * @brief Common implementation for ESP-BSP SDL abstraction layer
 */

#include "esp_bsp_sdl.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "esp_bsp_sdl";

// Forward declarations for board-specific functions
extern esp_err_t esp_bsp_sdl_board_init(esp_bsp_sdl_display_config_t *config, 
                                        esp_lcd_panel_handle_t *panel_handle,
                                        esp_lcd_panel_io_handle_t *panel_io_handle);
extern esp_err_t esp_bsp_sdl_board_backlight_on(void);
extern esp_err_t esp_bsp_sdl_board_backlight_off(void);
extern esp_err_t esp_bsp_sdl_board_display_on_off(bool enable);
extern esp_err_t esp_bsp_sdl_board_touch_init(void);
extern esp_err_t esp_bsp_sdl_board_touch_read(esp_bsp_sdl_touch_info_t *touch_info);
extern const char* esp_bsp_sdl_board_get_name(void);
extern esp_err_t esp_bsp_sdl_board_deinit(void);

esp_err_t esp_bsp_sdl_init(esp_bsp_sdl_display_config_t *config, 
                          esp_lcd_panel_handle_t *panel_handle,
                          esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing ESP-BSP SDL abstraction layer");
    ESP_LOGI(TAG, "Selected board: %s", esp_bsp_sdl_get_board_name());
    
    return esp_bsp_sdl_board_init(config, panel_handle, panel_io_handle);
}

esp_err_t esp_bsp_sdl_backlight_on(void)
{
    return esp_bsp_sdl_board_backlight_on();
}

esp_err_t esp_bsp_sdl_backlight_off(void)
{
    return esp_bsp_sdl_board_backlight_off();
}

esp_err_t esp_bsp_sdl_display_on_off(bool enable)
{
    return esp_bsp_sdl_board_display_on_off(enable);
}

esp_err_t esp_bsp_sdl_touch_init(void)
{
    return esp_bsp_sdl_board_touch_init();
}

esp_err_t esp_bsp_sdl_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
    if (!touch_info) {
        return ESP_ERR_INVALID_ARG;
    }
    
    return esp_bsp_sdl_board_touch_read(touch_info);
}

const char* esp_bsp_sdl_get_board_name(void)
{
    return esp_bsp_sdl_board_get_name();
}

esp_err_t esp_bsp_sdl_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing ESP-BSP SDL abstraction layer");
    return esp_bsp_sdl_board_deinit();
}