
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(RevisaoProject C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} 
        RevisaoProject.c 
        libs/display/ssd1306.c
        libs/led_matrix/led_matrix.c
        libs/buzzer/buzzer.c)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include/display
        ${CMAKE_CURRENT_LIST_DIR}/include/led_matrix
        ${CMAKE_CURRENT_LIST_DIR}/include/buzzer)

target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_pio
        hardware_clocks
        hardware_pwm
        hardware_i2c
        hardware_adc)

pico_add_extra_outputs(${PROJECT_NAME})