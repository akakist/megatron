cmake_minimum_required (VERSION 2.6)
add_definitions(-DWITH_IO)

add_library(ReferrerClient SHARED

referrerClient.cpp
referrerClientService.cpp
#../dfsReferrer/neighbours.cpp

)
if(ENABLE_QT_GUI)
target_link_libraries(ReferrerClient megatron-core  pthread ${Qt5Widgets_LIBRARIES})
else()
target_link_libraries(ReferrerClient megatron-core  pthread)
endif()
install(TARGETS ReferrerClient DESTINATION ${PLUGIN_TARGET_DIR})

