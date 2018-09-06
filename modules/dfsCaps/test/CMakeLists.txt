cmake_minimum_required (VERSION 2.6)

include_directories(../../lib/miniupnp-master)

add_library(test-caps SHARED
caps_test.cpp
caps_test_service.cpp
../CapsAlgorithm.cpp
)
target_link_libraries(test-caps megatron-main megatron-core miniupnpc z pthread dl)


install(TARGETS test-caps DESTINATION ${PLUGIN_TARGET_DIR})

