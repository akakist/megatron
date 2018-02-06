cmake_minimum_required (VERSION 2.6)



#include_directories(../local ../lib/jsoncpp-0.5.0/include /usr/local/include .. . )

add_library(megatron-main STATIC

CInstance.cpp
CUtils.cpp
configObj.cpp
megatron.cpp
threadNameCtl.cpp


)
#install(TARGETS megatron-main DESTINATION ${LIB_TARGET_DIR})

