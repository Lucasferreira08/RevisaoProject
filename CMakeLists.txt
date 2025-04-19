# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RevisaoProject C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${PROJECT_NAME} 
        RevisaoProject.c 
        libs/display/ssd1306.c
        libs/led_matrix/led_matrix.c
        libs/buzzer/buzzer.c)

# Adiciona o arquivo PIO
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# # Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include/display
        ${CMAKE_CURRENT_LIST_DIR}/include/led_matrix
        ${CMAKE_CURRENT_LIST_DIR}/include/buzzer)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_pio
        hardware_clocks
        hardware_pwm
        hardware_i2c
        hardware_adc)

pico_add_extra_outputs(${PROJECT_NAME})