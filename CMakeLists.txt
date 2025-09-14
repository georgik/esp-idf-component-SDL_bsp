# ESP-BSP SDL Abstraction Layer Component
# Board selection handled via Kconfig (sdkconfig) - keeps SDL component board-agnostic

set(srcs "src/esp_bsp_sdl_common.c")
set(board_deps "")

# Add board-specific source files and dependencies based on Kconfig selection
# These CONFIG_ variables are set by the sdkconfig.defaults.<board> files

# M5Stack Boards
if(CONFIG_ESP_BSP_SDL_BOARD_M5STACK_CORE_S3)
    list(APPEND srcs "src/boards/esp_bsp_sdl_m5stack_core_s3.c")
    list(APPEND board_deps "espressif__m5stack_core_s3_noglib")
    message(STATUS "ESP-BSP SDL: Building for M5Stack CoreS3 (320x240, QUAD PSRAM)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_M5_ATOM_S3)
    list(APPEND srcs "src/boards/esp_bsp_sdl_m5_atom_s3.c")
    list(APPEND board_deps "espressif__m5_atom_s3_noglib")
    message(STATUS "ESP-BSP SDL: Building for M5 Atom S3 (128x128, No PSRAM)")

# Espressif Development Boards
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP32_P4_FUNCTION_EV)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp32_p4_function_ev.c")
    list(APPEND board_deps "espressif__esp32_p4_function_ev_board_noglib")
    message(STATUS "ESP-BSP SDL: Building for ESP32-P4 Function EV (RISC-V, up to 1280x800)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP32_S3_LCD_EV)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp32_s3_lcd_ev.c")
    list(APPEND board_deps "espressif__esp32_s3_lcd_ev_board_noglib")
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3 LCD EV Board (Multiple LCD support)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP32_S3_EYE)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp32_s3_eye.c")
    list(APPEND board_deps "espressif__esp32_s3_eye_noglib")
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3 Eye (240x240, Camera, OCTAL PSRAM)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP32_S3_KORVO_2)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp32_s3_korvo_2.c")
    list(APPEND board_deps "espressif__esp32_s3_korvo_2_noglib")
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3 Korvo-2 (Audio, LCD, Camera)")

# Generic/Configurable BSPs (Universal DevKit Support)
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP_BSP_GENERIC)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp_bsp_generic.c")
    list(APPEND board_deps "esp_bsp_generic")
    message(STATUS "ESP-BSP SDL: Building for ESP BSP Generic (Configurable DevKit + Display)")
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP_BSP_DEVKIT)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp_bsp_devkit.c")
    list(APPEND board_deps "esp_bsp_devkit")
    message(STATUS "ESP-BSP SDL: Building for ESP BSP DevKit (LEDs/Buttons only)")

# Legacy/Deprecated Boards (kept for compatibility)
elseif(CONFIG_ESP_BSP_SDL_BOARD_ESP_BOX_3)
    list(APPEND srcs "src/boards/esp_bsp_sdl_esp_box_3.c")
    list(APPEND board_deps "esp-box-3_noglib")
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3-BOX-3 (320x240, OCTAL PSRAM)")
    
else()
    message(STATUS "ESP-BSP SDL: No board configuration found")
    message(WARNING "ESP-BSP SDL: No board selected - please run: idf.py -DBOARD=<board_name> build")
    message(STATUS "")
    message(STATUS "Available boards:")
    message(STATUS "  M5Stack: m5_atom_s3, m5stack_core_s3")
    message(STATUS "  Espressif: esp32_p4_function_ev, esp32_s3_lcd_ev, esp32_s3_eye, esp32_s3_korvo_2")
    message(STATUS "  Generic: esp_bsp_generic, esp_bsp_devkit")
    message(STATUS "  Legacy: esp_box_3")
    message(STATUS "")
    message(STATUS "See BOARDS.md for detailed board information.")
endif()

# Additional requirements based on target
set(extra_reqs "")
if(IDF_TARGET STREQUAL "esp32p4")
    list(APPEND extra_reqs esp_driver_ppa)
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include"
    REQUIRES ${board_deps}
    PRIV_REQUIRES ${board_deps} esp_lcd esp_lcd_touch ${extra_reqs}
)
