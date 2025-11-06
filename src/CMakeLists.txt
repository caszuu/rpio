# add optional drivers and capabilities
add_subdirectory(itf)
add_subdirectory(drv)

# build final fw binary
add_executable(rpio
    entry.c
    misc.c
)

pico_set_program_name(rpio "rpio")
pico_set_program_version(rpio "v0")

pico_enable_stdio_uart(rpio 0)
pico_enable_stdio_usb(rpio 0)

target_link_libraries(rpio
    hardware_watchdog
    pico_unique_id
    pico_multicore
    pico_stdlib

    rpio_proto
    itf_includes
    drv_includes
)

# link drivers with fw binary

target_link_libraries(rpio itf_spi)

if (RPIO_ENABLE_FB)
    target_link_libraries(rpio drv_fb)
endif()

if (RPIO_ENABLE_HUB75)
    target_link_libraries(rpio drv_hub75)
endif()

pico_add_extra_outputs(rpio)
