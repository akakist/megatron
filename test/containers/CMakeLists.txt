cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_library(test-containers SHARED
containers_test.cpp
containers_test_service.cpp
)
target_link_libraries(test-containers megatron-main megatron-core miniupnpc z pthread dl)


install(TARGETS test-containers DESTINATION ${PLUGIN_TARGET_DIR})

