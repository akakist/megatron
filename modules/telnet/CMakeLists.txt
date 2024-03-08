#cmake_minimum_required (VERSION 2.6)

add_library(core-telnet SHARED
    telnet.cpp
    telnetService.cpp
)

target_link_libraries(core-telnet megatron-core pthread)

install(TARGETS core-telnet DESTINATION ${PLUGIN_TARGET_DIR})

