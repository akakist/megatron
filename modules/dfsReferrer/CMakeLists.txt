#cmake_minimum_required (VERSION 2.6)


add_library(dfs-referrer SHARED

dfsReferrer.cpp
dfsReferrerService.cpp
#neighbours.cpp
uplinkconnectionstate.cpp
uriReferals.cpp
dfsReferrerService.h
uplinkconnectionstate.h
uriReferals.h

)
target_link_libraries(dfs-referrer megatron-core pthread)

install(TARGETS dfs-referrer DESTINATION ${PLUGIN_TARGET_DIR})

