cmake_minimum_required(VERSION 3.12)

set(FREERTOS_PORT GCC_ARM_CM0 CACHE STRING "")
set(FREERTOS_HEAP 4)


# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(pico_emb C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Define freertos_config target
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/freertos  # Path to FreeRTOSConfig.h
    ${CMAKE_CURRENT_LIST_DIR}/freertos/FreeRTOS-Kernel/include
    ${CMAKE_CURRENT_LIST_DIR}/freertos/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${CMAKE_CURRENT_LIST_DIR}/freertos/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
)


if(PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
  message(
    FATAL_ERROR
      "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}"
  )
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(
  -Wall -Wno-format # int != int32_t as far as the compiler is concerned because
                    # gcc has int32_t as long int
  -Wno-unused-function # we have some for the docs that aren't called
  -DprojCOVERAGE_TEST=0 # Add coverage test definition here instead of freertos_config
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wno-maybe-uninitialized)
endif()

# Codes subdir:
add_subdirectory(oled1_lib)
add_subdirectory(freertos/FreeRTOS-Kernel)
add_subdirectory(main)
