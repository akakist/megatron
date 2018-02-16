cmake_minimum_required (VERSION 2.6)


link_directories(/usr/local/mysql/lib)

add_library(core-mysql SHARED
    mysql.cpp
    mysqlService.cpp
)

target_link_libraries(core-mysql megatron-core mysqlclient pthread)

install(TARGETS core-mysql DESTINATION ${PLUGIN_TARGET_DIR})

