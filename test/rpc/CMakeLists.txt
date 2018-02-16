cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_library(core-rpc-test SHARED
    rpc_test.cpp
    rpc_test_service.cpp
)
target_link_libraries(core-rpc-test megatron-main megatron-core miniupnpc z pthread dl)


install(TARGETS core-rpc-test DESTINATION ${PLUGIN_TARGET_DIR})

