cmake_minimum_required (VERSION 2.6)

add_library(core-sqlite SHARED

sqlite.cpp
sqliteService.cpp

)
target_link_libraries(core-sqlite megatron-core)
install(TARGETS core-sqlite DESTINATION ${PLUGIN_TARGET_DIR})

