cmake_minimum_required (VERSION 2.6)
add_definitions(-DWITH_IO)

add_library(dfs-referrer SHARED

../../corelib/sqlite3Wrapper.cpp
dfsReferrer.cpp
dfsReferrerService.cpp
neighbours.cpp
uplinkconnectionstate.cpp
uriReferals.cpp

)
target_link_libraries(dfs-referrer megatron-core sqlite3 pthread)
install(TARGETS dfs-referrer DESTINATION ${PLUGIN_TARGET_DIR})

