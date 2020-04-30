cmake_minimum_required(VERSION 3.0)

#project name
project(tests)

add_subdirectory(${PROJECT_SOURCE_DIR}/uart)
add_subdirectory(${PROJECT_SOURCE_DIR}/small)
add_subdirectory(${PROJECT_SOURCE_DIR}/ezsp)