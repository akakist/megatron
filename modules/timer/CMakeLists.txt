cmake_minimum_required (VERSION 2.6)

add_library(core-timer SHARED
    timer.cpp
    timerService.cpp
)

target_link_libraries(core-timer megatron-core)

install(TARGETS core-timer DESTINATION ${PLUGIN_TARGET_DIR})

