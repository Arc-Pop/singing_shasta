# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Set top source code folder
set(SOURCE_FOLDER "src")

project(shasta C CXX ASM)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(shasta 
src/singing_shasta.c 
src/lib/yinShasta.c  
src/lib/pitch.c 
src/lib/bluetooth/bt_audio.c
src/lib/bluetooth/hxcmod-player/hxcmod.c
src/lib/bluetooth/hxcmod-player/mods/nao-deceased_by_disease.c
src/lib/sd_card/sd_audio.c
)

pico_set_program_name(shasta "shasta")
pico_set_program_version(shasta "0.1")

# Generate PIO header
#pico_generate_pio_header(shasta ${CMAKE_CURRENT_LIST_DIR}/blink.pio)
file(GLOB_RECURSE pio_src CONFIGURE_DEPENDS "${SOURCE_FOLDER}/*.pio")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(shasta 1)
pico_enable_stdio_usb(shasta 0)

# Add the standard library to the build
target_link_libraries(shasta
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(shasta PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  lib
)

target_compile_definitions(shasta PRIVATE
        AUDIO_FREQ_MAX=48000BTSTACK_ROOT

        # ours are zero based, so say so
        PICO_USBDEV_USE_ZERO_BASED_INTERFACES=1

        # need large descriptor
        PICO_USBDEV_MAX_DESCRIPTOR_SIZE=256


        PICO_USBDEV_ISOCHRONOUS_BUFFER_STRIDE_TYPE=1
        PICO_USBDEV_ENABLE_DEBUG_TRACE

        CYW43_LWIP=0
)

set(BTSTACK_3RD_PARTY_PATH ${PICO_SDK_PATH}/lib/btstack/3rd-party)

set(BTSTACK_EXAMPLE_PATH ${BTSTACK_ROOT}/example)
# mod player used by a2dp_source_demo and mod_player and demo song
add_library(pico_btstack_hxcmod_player INTERFACE)
target_sources(pico_btstack_hxcmod_player INTERFACE
        ${BTSTACK_3RD_PARTY_PATH}/hxcmod-player/hxcmod.c
        ${BTSTACK_3RD_PARTY_PATH}/hxcmod-player/mods/nao-deceased_by_disease.c
        )
target_include_directories(pico_btstack_hxcmod_player INTERFACE
        ${BTSTACK_3RD_PARTY_PATH}/hxcmod-player
        ${BTSTACK_3RD_PARTY_PATH}/hxcmod-player/mods
        )
add_library(pico_btstack_sco_demo_util INTERFACE)
target_sources(pico_btstack_sco_demo_util INTERFACE
        # sco demo utils
        ${BTSTACK_EXAMPLE_PATH}/sco_demo_util.c
        )

# Add any user requested libraries
target_link_libraries(shasta
        pico_stdlib 
        hardware_spi
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_gpio
        hardware_watchdog
        pico_cyw43_arch_none
        hardware_adc
        pico_btstack_classic
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_threadsafe_background
        pico_btstack_sbc_encoder
        usb_device
        pico_multicore
        hardware_uart
        pico_fatfs
        )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(shasta)

if (NOT TARGET pico_fatfs)
    add_library(pico_fatfs INTERFACE)

    target_sources(pico_fatfs INTERFACE
        ${SOURCE_FOLDER}/lib/sd_card/fatfs/ff.c
        ${SOURCE_FOLDER}/lib/sd_card/fatfs/ffsystem.c
        ${SOURCE_FOLDER}/lib/sd_card/fatfs/ffunicode.c
        ${SOURCE_FOLDER}/lib/sd_card/tf_card.c
    )

    target_include_directories(pico_fatfs INTERFACE
        ${SOURCE_FOLDER}/lib/sd_card
        ${SOURCE_FOLDER}/lib/sd_card/fatfs
        ${SOURCE_FOLDER}/lib/sd_card/fatfs/conf
    )

    target_link_libraries(pico_fatfs INTERFACE
        pico_stdlib
        hardware_clocks
        hardware_spi
    )
endif()