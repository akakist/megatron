cmake_minimum_required (VERSION 2.6)
add_definitions(-DWITH_IO)

add_library(ReferrerClient SHARED
referrerClient.cpp
referrerClientService.cpp
../dfsReferrer/neighbours.cpp

)
target_link_libraries(ReferrerClient megatron-core sqlite3)
install(TARGETS ReferrerClient DESTINATION ${PLUGIN_TARGET_DIR})

