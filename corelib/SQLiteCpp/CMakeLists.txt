cmake_minimum_required (VERSION 2.6)

#project(dfsCore)


#include_directories(../local ../lib/jsoncpp-0.5.0/include /usr/local/include .. . )
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

