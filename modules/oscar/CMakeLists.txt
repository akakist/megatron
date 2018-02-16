cmake_minimum_required (VERSION 2.6)

add_library(core-oscar SHARED
    oscar.cpp
    oscarService.cpp
)

target_link_libraries(core-oscar megatron-core pthread)

install(TARGETS core-oscar DESTINATION ${PLUGIN_TARGET_DIR})

