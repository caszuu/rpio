# add optional drivers and capabilities

add_subdirectory(itf/spi)

if (RPIO_ENABLE_HUB75)
    add_subdirectory(drv/hub75)
endif()

# build final fw binary
add_executable(rpio
    entry.c
)

pico_set_program_name(rpio "rpio")
pico_set_program_version(rpio "v0")

pico_enable_stdio_uart(rpio 0)
pico_enable_stdio_usb(rpio 0)

target_include_directories(rpio PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/aux
)

target_link_libraries(rpio
    hardware_watchdog
)

# link drivers with fw binary

target_link_libraries(rpio itf_spi)

if (RPIO_ENABLE_HUB75)
    target_link_libraries(rpio drv_hub75)
endif()

pico_add_extra_outputs(rpio)
