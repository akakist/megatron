#cmake_minimum_required (VERSION 2.6)


add_library(core-error-dispatcher SHARED
errorDispatcher.cpp
errorDispatcherService.cpp
)
target_link_libraries(core-error-dispatcher megatron-core pthread)
install(TARGETS core-error-dispatcher DESTINATION ${PLUGIN_TARGET_DIR})

