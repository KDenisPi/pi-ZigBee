cmake_minimum_required(VERSION 3.0.0)
project(pi-zigbee-lib VERSION 0.1.0)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../pi-utils
)

add_library(pi-zigbee-lib STATIC uart.cpp ezsp.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

