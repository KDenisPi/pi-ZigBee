cmake_minimum_required(VERSION 3.0)

#project name
project(test-uart)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
)


add_executable(t_uart test_uart.cpp)
add_executable(t_uart_frame_esc uart_frame_escape.cpp)

set(TEST_UART_LIBS pi-zigbee-lib pi-utils pthread crypt rt)

target_link_libraries(t_uart ${TEST_UART_LIBS})
target_link_libraries(t_uart_frame_esc ${TEST_UART_LIBS})

#add_test(NAME UART COMMAND sudo t_uart)
add_test(NAME UART_Frame_ESC COMMAND sudo t_uart_frame_esc)