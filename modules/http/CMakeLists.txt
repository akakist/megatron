cmake_minimum_required (VERSION 2.6)


add_library(core-http SHARED

http.cpp
httpService.cpp
mime_types.cpp

)
target_link_libraries(core-http megatron-core pthread)
install(TARGETS core-http DESTINATION ${PLUGIN_TARGET_DIR})

