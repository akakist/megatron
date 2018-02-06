cmake_minimum_required (VERSION 2.6)

include_directories(../../../dfs_lib/lib/miniupnp-master)
add_library(core-rpc SHARED
    rpc.cpp
    rpcService.cpp
    upnp_tr.c
)

add_library(core-rpc-test SHARED
    rpc_test.cpp
    rpc_test_service.cpp
)
target_link_libraries(core-rpc megatron-core miniupnpc)
target_link_libraries(core-rpc-test megatron-main megatron-core miniupnpc z)

install(TARGETS core-rpc DESTINATION ${PLUGIN_TARGET_DIR})

install(TARGETS core-rpc-test DESTINATION ${PLUGIN_TARGET_DIR})

