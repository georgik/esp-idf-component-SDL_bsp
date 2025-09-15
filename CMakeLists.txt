# ESP-BSP SDL Abstraction Layer Component
# Kconfig-based BSP selection to avoid header conflicts

# Include all board-specific sources - conditional compilation will handle selection
set(COMPONENT_SRCS 
    "src/esp_bsp_sdl_common.c"
    "src/boards/esp_bsp_sdl_m5_atom_s3.c"
    "src/boards/esp_bsp_sdl_m5stack_core_s3.c"
    "src/boards/esp_bsp_sdl_esp_box_3.c"
)

# Include only the required BSP dependencies - this is the minimum required
set(COMPONENT_REQUIRES "esp_lcd")
set(COMPONENT_PRIV_REQUIRES "espressif__esp_lcd_touch")

# Debug: Add ESP-BOX-3 BSP unconditionally to test
list(APPEND COMPONENT_PRIV_REQUIRES "espressif__esp-box-3_noglib")

# Conditionally include the selected BSP (commented out for debugging)
# if(CONFIG_ESP_BSP_SDL_BOARD_M5_ATOM_S3)
#     list(APPEND COMPONENT_PRIV_REQUIRES "espressif__m5_atom_s3_noglib")
# elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP_BOX_3)
#     list(APPEND COMPONENT_PRIV_REQUIRES "espressif__esp-box-3_noglib")
# elseif(CONFIG_ESP_BSP_SDL_BOARD_M5STACK_CORE_S3)
#     list(APPEND COMPONENT_PRIV_REQUIRES "espressif__m5stack_core_s3")
# else()
#     message(WARNING "ESP-BSP SDL: No BSP selected or unsupported board configuration")
# endif()

# Register the component
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "include"
    REQUIRES ${COMPONENT_REQUIRES}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
)

# Add board selection messages during build (after CONFIG variables are available)
if(CONFIG_ESP_BSP_SDL_BOARD_M5_ATOM_S3)
    message(STATUS "ESP-BSP SDL: Building for M5 Atom S3 (128x128, no PSRAM)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_M5STACK_CORE_S3)
    message(STATUS "ESP-BSP SDL: Building for M5Stack CoreS3 (320x240, QUAD PSRAM)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP_BOX_3)
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3-BOX-3 (320x240, OCTAL PSRAM)")
else()
    message(WARNING "ESP-BSP SDL: No specific board detected in configuration. Check menuconfig.")
endif()
