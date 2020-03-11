cmake_minimum_required (VERSION 2.6)

#project(dfsCore)


include_directories(~/work/megatron/lib/sqlite3 )
#add_definitions(-std=gnu++0x)

add_library(sqlitecpp STATIC
Backup.cpp
Column.cpp
Database.cpp
Exception.cpp
Statement.cpp
Transaction.cpp


           )
#install(TARGETS sqlitecpp DESTINATION ${LIB_TARGET_DIR})

