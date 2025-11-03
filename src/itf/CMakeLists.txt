# == includes ==

add_library(itf_includes INTERFACE)
target_include_directories(itf_includes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# == spi ==

if (RPIO_ENABLE_SPI)
    add_library(itf_spi spi.c)

    target_compile_definitions(itf_spi PUBLIC RPIO_INTERFACE_SPI)
    target_link_libraries(itf_spi PRIVATE hardware_gpio hardware_spi)
endif()
