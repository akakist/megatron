#cmake_minimum_required (VERSION 2.6)

ADD_SUBDIRECTORY(test)

add_library(dfs-referrer SHARED

dfsReferrer.cpp
dfsReferrerService.cpp
#neighbours.cpp
uplinkconnectionstate.cpp
uriReferals.cpp

)
target_link_libraries(dfs-referrer megatron-core pthread)

install(TARGETS dfs-referrer DESTINATION ${PLUGIN_TARGET_DIR})

