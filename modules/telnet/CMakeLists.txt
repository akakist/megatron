#cmake_minimum_required (VERSION 2.6)

add_library(core-telnet SHARED
    telnet.cpp
    telnetService.cpp
telnet.h
telnetService.h
telnet_keys.h
)

target_link_libraries(core-telnet megatron-core pthread)

install(TARGETS core-telnet DESTINATION ${PLUGIN_TARGET_DIR})

