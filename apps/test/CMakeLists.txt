cmake_minimum_required (VERSION 2.6)

add_executable(mt-test-${PORT}.exe 
mainTest.cpp
../../main/CInstance.cpp
../../main/CUtils.cpp
../../main/configObj.cpp
../../main/megatron.cpp
../../main/threadNameCtl.cpp


)
include_directories(../../main)
#add_definitions(-DWITHOUT_UPNP )

target_link_libraries(mt-test-${PORT}.exe zsingle megatron-core ssl crypto  z dl pthread)
install(TARGETS mt-test-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

