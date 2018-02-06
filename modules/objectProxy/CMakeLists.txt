cmake_minimum_required (VERSION 2.6)

add_library(core-objectProxy SHARED
    objectProxy.cpp
    objectProxyService.cpp
)

target_link_libraries(core-objectProxy megatron-core)

install(TARGETS core-objectProxy DESTINATION ${PLUGIN_TARGET_DIR})

