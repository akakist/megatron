#cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_library(test-rpc SHARED
    rpc_test.cpp
    rpc_test_service.cpp
)
target_link_libraries(test-rpc megatron-main megatron-core  z pthread)


install(TARGETS test-rpc DESTINATION ${PLUGIN_TARGET_DIR})

