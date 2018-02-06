cmake_minimum_required (VERSION 2.6)

add_executable(mt-main-${PORT}.exe 
mainApp.cpp
../../main/CInstance.cpp
../../main/CUtils.cpp
../../main/configObj.cpp
../../main/megatron.cpp
../../main/threadNameCtl.cpp


)
include_directories(../../main)

target_link_libraries(mt-main-${PORT}.exe megatron-core z dl pthread)
install(TARGETS mt-main-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

