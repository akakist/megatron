#cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_library(test-udp SHARED
    udp_test.cpp
    udp_test_service.cpp
)
target_link_libraries(test-udp megatron-main megatron-core z pthread)


install(TARGETS test-udp DESTINATION ${PLUGIN_TARGET_DIR})

