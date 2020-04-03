cmake_minimum_required(VERSION 3.0.0)
project(pi-zigbee VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -Wno-psabi")

message(STATUS "CMAKE_CXX_KNOWN_FEATURES is ${CMAKE_CXX_KNOWN_FEATURES}")
message(STATUS "CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-zigbee-lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

