cmake_minimum_required (VERSION 2.6)
add_definitions(-DWITH_IO)

add_library(dfs-referrer SHARED

dfsReferrer.cpp
dfsReferrerService.cpp
#neighbours.cpp
uplinkconnectionstate.cpp
uriReferals.cpp

)
IF(ENABLE_QT_GUI)
target_link_libraries(dfs-referrer megatron-core pthread ${Qt5Widgets_LIBRARIES})
ELSE()
target_link_libraries(dfs-referrer megatron-core pthread)
ENDIF()
install(TARGETS dfs-referrer DESTINATION ${PLUGIN_TARGET_DIR})

