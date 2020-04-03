cmake_minimum_required(VERSION 3.0)

#project name
project(test-uart)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
)


add_executable(t_uart test_uart.cpp)

set(TEST_UART_LIBS pi-zigbee-lib pi-utils pthread crypt rt)

target_link_libraries(t_uart ${TEST_UART_LIBS})

add_test(NAME UART COMMAND sudo t_uart)