cmake_minimum_required(VERSION 3.0)

#project name
project(test-ezsp)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
)


add_executable(t_frame ezsp_load_put.cpp)

set(TEST_EZSP_LIBS pi-zigbee-lib pi-utils pthread crypt rt)

target_link_libraries(t_frame ${TEST_EZSP_LIBS})

add_test(NAME EZSP_Frame_Load_Put COMMAND t_frame)
