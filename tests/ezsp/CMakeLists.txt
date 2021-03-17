cmake_minimum_required(VERSION 3.0)

#project name
project(test-ezsp)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-zigbee-lib
    ${PROJECT_SOURCE_DIR}/../../../third-party/json/include
)


add_executable(t_frame ezsp_load_put.cpp)
add_executable(t_ezsp_svc ezsp_svc.cpp)
add_executable(t_json_load ezsp_cfg_json.cpp)
add_executable(t_zcl ezsp_zcl_frame.cpp)
add_executable(t_json_save ezsp_cfg_json_save.cpp)

set(TEST_EZSP_LIBS pi-zigbee-lib pi-utils pthread crypt rt)

target_link_libraries(t_frame ${TEST_EZSP_LIBS})
target_link_libraries(t_ezsp_svc ${TEST_EZSP_LIBS})
target_link_libraries(t_json_load ${TEST_EZSP_LIBS})
target_link_libraries(t_zcl ${TEST_EZSP_LIBS})
target_link_libraries(t_json_save ${TEST_EZSP_LIBS})

add_test(NAME EZSP_Frame_Load_Put COMMAND t_frame)
add_test(NAME EZSP_SVC COMMAND t_ezsp_svc)
add_test(NAME EZSP_JSON COMMAND t_json_load)
add_test(NAME EZSP_JSON_SAVE COMMAND t_json_save)
