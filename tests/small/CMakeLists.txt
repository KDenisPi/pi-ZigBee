cmake_minimum_required(VERSION 3.0)

#project name
project(test-small)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
)


add_executable(t_small small.cpp)
add_executable(t_times times.cpp)

set(TEST_LIBS pi-utils pi-zigbee-lib pthread crypt rt)

target_link_libraries(t_small ${TEST_LIBS})
target_link_libraries(t_times ${TEST_LIBS})
