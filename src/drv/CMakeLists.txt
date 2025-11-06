# == includes ==

add_library(drv_includes INTERFACE)
target_include_directories(drv_includes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# == fb ==

if (RPIO_ENABLE_FB)
    add_library(drv_fb
        fb/fb.c
    )

    target_link_libraries(drv_fb PRIVATE
        pico_platform
        
        rpio_proto
        drv_includes
        itf_includes
    )
endif()

# == hub75 ==

if (RPIO_ENABLE_HUB75)
    add_library(drv_hub75
        hub75/hub.c
    )

    pico_generate_pio_header(drv_hub75 ${CMAKE_CURRENT_SOURCE_DIR}/hub75/hub75.pio)
    target_link_libraries(drv_hub75 PRIVATE
        hardware_pio
        hardware_dma
        hardware_irq
        drv_fb

        rpio_proto
        drv_includes
        itf_includes
    )
endif()
