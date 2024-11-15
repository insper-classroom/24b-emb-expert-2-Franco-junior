add_executable(main main.c)

# pull in common dependencies
target_link_libraries(main pico_stdlib hardware_dma oled1_lib hardware_adc FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 freertos_config)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)
