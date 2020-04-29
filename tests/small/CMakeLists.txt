cmake_minimum_required(VERSION 3.0)

#project name
project(test-small)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
)


add_executable(t_small small.cpp)

set(TEST_LIBS pthread crypt rt)

target_link_libraries(t_small ${TEST_LIBS})
