cmake_minimum_required (VERSION 2.6)
add_library(dfs-caps SHARED

dfsCaps.cpp
dfsCapsService.cpp
NetTreeRoute.cpp
)
target_link_libraries(dfs-caps megatron-core  sqlite3)
install(TARGETS dfs-caps DESTINATION ${PLUGIN_TARGET_DIR})

