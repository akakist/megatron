#cmake_minimum_required (VERSION 2.6)




add_library(megatron-main STATIC

CInstance.cpp
CUtils.cpp
configObj.cpp
megatron.cpp
threadNameCtl.cpp
megatronClient.cpp
iutils_decl.cpp

)
#install(TARGETS megatron-main DESTINATION ${LIB_TARGET_DIR})

