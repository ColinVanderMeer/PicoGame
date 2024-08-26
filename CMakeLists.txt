# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(PicoGame C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(PicoGame main.c )

pico_set_program_name(PicoGame "PicoGame")
pico_set_program_version(PicoGame "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(PicoGame 0)
pico_enable_stdio_usb(PicoGame 1)

# Add the standard library to the build
target_link_libraries(PicoGame
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(PicoGame PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(PicoGame 
        hardware_spi
        hagl
        hagl_hal
        mod_player
        )

add_subdirectory(external/hagl)
add_subdirectory(external/hagl_hal)
add_subdirectory(external/mod_player)

pico_add_extra_outputs(PicoGame)

target_compile_definitions(PicoGame PRIVATE
#  HAGL_HAL_USE_SINGLE_BUFFER
  HAGL_HAL_USE_DOUBLE_BUFFER
#  HAGL_HAL_USE_TRIPLE_BUFFER
#  HAGL_HAL_USE_DMA
  HAGL_HAL_DEBUG
#  HAGL_HAL_PIXEL_SIZE=2
)

target_compile_definitions(PicoGame PRIVATE
MIPI_DISPLAY_PIN_CS=20
MIPI_DISPLAY_PIN_DC=22
MIPI_DISPLAY_PIN_RST=26
MIPI_DISPLAY_PIN_BL=17
MIPI_DISPLAY_PIN_CLK=18
MIPI_DISPLAY_PIN_MOSI=19
MIPI_DISPLAY_PIN_MISO=16

MIPI_DISPLAY_SPI_PORT=spi0
MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=68000000
MIPI_DISPLAY_INVERT=0

MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT
MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_RGB
MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_SWAP_XY|MIPI_DCS_ADDRESS_MODE_SWAP_XY|MIPI_DCS_ADDRESS_MODE_MIRROR_X
MIPI_DISPLAY_WIDTH=160
MIPI_DISPLAY_HEIGHT=128
MIPI_DISPLAY_OFFSET_X=0
MIPI_DISPLAY_OFFSET_Y=0

)
