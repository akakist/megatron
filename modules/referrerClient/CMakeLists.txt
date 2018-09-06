cmake_minimum_required (VERSION 2.6)
add_definitions(-DWITH_IO)

add_library(ReferrerClient SHARED
../../corelib/sqlite3Wrapper.cpp

referrerClient.cpp
referrerClientService.cpp
#../dfsReferrer/neighbours.cpp

)
target_link_libraries(ReferrerClient megatron-core sqlite3 pthread)
install(TARGETS ReferrerClient DESTINATION ${PLUGIN_TARGET_DIR})

