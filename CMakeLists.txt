if (NOT TARGET pico_mux74hc4067)
    add_library(pico_mux74hc4067 INTERFACE)

    target_sources(pico_mux74hc4067 INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/MUX74HC4067.c
    )

    target_include_directories(pico_mux74hc4067 INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}
    )

    target_link_libraries(pico_mux74hc4067 INTERFACE
            pico_stdlib
            hardware_adc
            hardware_gpio
    )
endif()