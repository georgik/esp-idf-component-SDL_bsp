# ESP-BSP-SDL Abstraction Layer

This component provides a board-agnostic abstraction layer between SDL and ESP-BSP (Board Support Package). It solves the problem of having to specify board configurations via command-line parameters by moving board selection into ESP-IDF's native Kconfig system.

## Problem Solved

**Before**: Developers had to use complex command-line syntax for every build:
```bash
idf.py @boards/esp-box-3.cfg build flash monitor
```

**After**: Simple, standard ESP-IDF workflow:
```bash
idf.py menuconfig  # Select board once
idf.py build flash monitor  # Use for all subsequent builds
```

## Architecture

```
┌─────────────────┐
│  SDL Application │
└─────────┬───────┘
          │
┌─────────▼───────┐
│  SDL Component  │ (Board-agnostic)
└─────────┬───────┘
          │
┌─────────▼───────┐
│ ESP-BSP-SDL     │ (Abstraction Layer)
│ Component       │
└─────────┬───────┘
          │
┌─────────▼───────┐
│ ESP-BSP         │ (Board-specific)
│ Components      │
└─────────────────┘
```

## Key Benefits

1. **Board Agnostic**: SDL no longer needs to know about specific boards
2. **Clean Separation**: BSP-specific code is isolated in the abstraction layer  
3. **Easy Configuration**: Board selection through menuconfig, no command-line args needed
4. **Maintainable**: Adding new boards only requires implementing the abstraction interface
5. **Portable**: The same SDL code works across all supported boards
6. **Flexible**: Easy to switch boards without changing build commands

## Usage

### 1. Select Your Board
```bash
idf.py menuconfig
```
Navigate to: `ESP-BSP SDL Configuration` → `Select Target Board`

Choose from supported boards:
- ESP-Box-3
- ESP-Box
- ESP32-C6 DevKit with ILI9341 Display  
- M5Stack Core S3
- ESP32-C3-LCDkit
- ESP32-P4 Function EV Board

### 2. Build and Flash
```bash
idf.py build flash monitor
```

That's it! No need to specify `@boards/BOARD.cfg` anymore.

### 3. Switching Boards
To switch to a different board:
1. Run `idf.py menuconfig`
2. Change the board selection
3. Run `idf.py build flash monitor`

The abstraction layer will automatically handle the board-specific configuration.

## Implementation Details

### Component Structure
```
components/esp_bsp_sdl/
├── include/
│   └── esp_bsp_sdl.h          # Public API
├── src/
│   ├── esp_bsp_sdl_common.c   # Common implementation
│   └── boards/                # Board-specific implementations
│       ├── esp_bsp_sdl_esp_box_3.c
│       ├── esp_bsp_sdl_esp_box.c
│       └── ...
├── Kconfig                    # Board selection configuration
└── CMakeLists.txt            # Build configuration
```

### API Functions
- `esp_bsp_sdl_init()` - Initialize display and touch for selected board
- `esp_bsp_sdl_backlight_on/off()` - Control display backlight
- `esp_bsp_sdl_display_on_off()` - Enable/disable display
- `esp_bsp_sdl_touch_init/read()` - Touch interface (if supported)
- `esp_bsp_sdl_get_board_name()` - Get current board name
- `esp_bsp_sdl_deinit()` - Cleanup resources

### Board-Specific Implementation
Each supported board has its own implementation file that:
1. Includes the appropriate ESP-BSP headers
2. Implements the abstraction API functions
3. Handles board-specific initialization and configuration
4. Provides display parameters (resolution, pixel format, etc.)
5. Manages touch interface (if available)

## Adding New Boards

To add support for a new board:

1. **Update Kconfig** (`Kconfig`):
   ```kconfig
   config ESP_BSP_SDL_BOARD_NEW_BOARD
       bool "New Board Name"
       help
           Description of the new board.
   ```

2. **Update CMakeLists.txt**:
   ```cmake
   elseif(CONFIG_ESP_BSP_SDL_BOARD_NEW_BOARD)
       list(APPEND srcs "src/boards/esp_bsp_sdl_new_board.c")
       list(APPEND board_deps "new_board_bsp_component")
   ```

3. **Create board implementation** (`src/boards/esp_bsp_sdl_new_board.c`):
   - Include board-specific BSP headers
   - Implement all required abstraction API functions
   - Handle board-specific display and touch initialization

## Migration from Old Approach

### Old SDL Integration
```c
// Direct BSP includes
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

// BSP constants used directly
mode.w = BSP_LCD_H_RES;
mode.h = BSP_LCD_V_RES;

// Direct BSP calls
ESP_ERROR_CHECK(bsp_display_new(&bsp_disp_cfg, &panel_handle, &panel_io_handle));
ESP_ERROR_CHECK(bsp_display_backlight_on());
```

### New SDL Integration
```c
// Abstraction layer include
#include "esp_bsp_sdl.h"

// Get configuration through abstraction
esp_bsp_sdl_display_config_t config;
esp_bsp_sdl_init(&config, &panel_handle, &panel_io_handle);
mode.w = config.width;
mode.h = config.height;

// Abstracted function calls
ESP_ERROR_CHECK(esp_bsp_sdl_backlight_on());
```

## Supported Boards

Currently implemented boards:
- ✅ ESP-Box-3 (`esp-box-3_noglib`)
- 🚧 ESP-Box (`esp-box_noglib`) - Template provided
- 🚧 ESP32-C6 DevKit (`esp_bsp_generic`) - Template provided  
- 🚧 M5Stack Core S3 (`m5stack_core_s3`) - Template provided
- 🚧 ESP32-C3-LCDkit (`esp32_c3_lcdkit`) - Template provided
- 🚧 ESP32-P4 Function EV (`esp32_p4_function_ev_board`) - Template provided

Legend: ✅ Fully implemented, 🚧 Template provided (needs completion)

## Dependencies

The abstraction layer automatically manages dependencies based on the selected board:
- ESP-BSP component for the selected board
- `esp_lcd` for LCD operations
- `esp_driver_ppa` (for ESP32-P4 targets)
- Touch-related components (when supported by board)

## Critical Configuration Settings

### ESP-Box-3 Requirements

For ESP-Box-3 to work properly, these configurations are **mandatory**:

```ini
# Hardware Requirements
CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y      # 8MB flash memory
CONFIG_SPIRAM_MODE_OCT=y              # Octal PSRAM mode (NOT quad!)
CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y # 240MHz for optimal performance

# 🔧 CRITICAL DEBUG Setting 🔧
CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT=y   # Halt on panic instead of reboot
                                       # Essential for debugging!
```

### Why `CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT=y` is Essential

**Without this setting**: 
- System reboots continuously on panic/crash
- Makes debugging nearly impossible
- You see endless reboot loops

**With this setting**:
- System halts after printing complete panic information
- Stack traces are fully visible
- Easy to identify crash locations
- Memory issues become debuggable

This setting is **critical** for development and should always be enabled during the development phase.

## Configuration Files

The old approach required separate configuration files for each board:
- `sdkconfig.defaults.esp-box-3`
- `sdkconfig.defaults.esp32_c6_devkit`
- etc.

These files are still used internally by the ESP-BSP components, but developers no longer need to manually specify them via command-line arguments. However, the main `sdkconfig.defaults` should contain the critical settings mentioned above.

## Troubleshooting

### Build Errors
- Ensure you've selected a board via `idf.py menuconfig`
- Check that the ESP-BSP component for your board is available
- Verify that your target matches the selected board (e.g., ESP32-S3 for ESP-Box-3)

### Display Issues  
- Verify board selection matches your hardware
- Check display connections and power supply
- Review board-specific documentation for pin configurations

### Touch Not Working
- Ensure your board supports touch (check `has_touch` in display config)
- Verify touch interface initialization in board-specific implementation
- Check touch hardware connections