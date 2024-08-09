#cmake_minimum_required (VERSION 2.6)


add_library(core-logger SHARED
logger.cpp
loggerService.cpp
loggerService.h
)
target_link_libraries(core-logger megatron-core pthread)
install(TARGETS core-logger DESTINATION ${PLUGIN_TARGET_DIR})

