cmake_minimum_required(VERSION 3.16)

# ESP-BSP SDL Abstraction Layer Component
# Kconfig-based BSP selection to avoid header conflicts

# Conditional source files based on board selection to avoid compilation errors
set(COMPONENT_SRCS "src/esp_bsp_sdl_common.c")

# Add board-specific sources based on build configuration
if(DEFINED ENV{BUILD_FOR_BOX3} OR "$ENV{BUILD_FOR_BOX3}" STREQUAL "1")
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_esp_box_3.c")
    message(STATUS "ESP-BSP SDL: Including ESP32-S3-BOX-3 source files")
else()
    # Default to M5 Atom S3
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_m5_atom_s3.c")
    message(STATUS "ESP-BSP SDL: Including M5 Atom S3 source files")
endif()

# Include only the required BSP dependencies - this is the minimum required
set(COMPONENT_REQUIRES "esp_lcd")
set(COMPONENT_PRIV_REQUIRES "espressif__esp_lcd_touch")

# Conditional BSP selection to avoid symbol conflicts
# Both M5 Atom S3 and ESP-BOX-3 BSPs define identical function names
# We include only the required BSP based on build configuration

# Check if we're building for ESP32-S3-BOX-3
if(DEFINED ENV{BUILD_FOR_BOX3} OR "$ENV{BUILD_FOR_BOX3}" STREQUAL "1")
    list(APPEND COMPONENT_PRIV_REQUIRES "espressif__esp-box-3_noglib")
    message(STATUS "ESP-BSP SDL: Including ESP32-S3-BOX-3 BSP (M5 Atom S3 excluded to prevent conflicts)")
else()
    # Default to M5 Atom S3
    list(APPEND COMPONENT_PRIV_REQUIRES "espressif__m5_atom_s3_noglib")
    message(STATUS "ESP-BSP SDL: Including M5 Atom S3 BSP (ESP-BOX-3 excluded to prevent conflicts)")
endif()

# This approach ensures:
# 1. All BSP headers are available during compilation
# 2. No dependency resolution timing issues
# 3. Board selection works purely through Kconfig + conditional compilation
# 4. Simple addition of new boards by adding to the list above

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
