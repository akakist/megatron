cmake_minimum_required (VERSION 2.6)

add_library(core-webHandler SHARED
    webHandler.cpp
    webHandlerService.cpp
)

target_link_libraries(core-webHandler megatron-core)

install(TARGETS core-webHandler DESTINATION ${PLUGIN_TARGET_DIR})

