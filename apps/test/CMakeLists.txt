cmake_minimum_required (VERSION 2.6)

add_executable(mt-test-${PORT}.exe 
mainTest.cpp
#../../main/CInstance.cpp
#../../main/CUtils.cpp
#../../main/configObj.cpp
#../../main/megatron.cpp
#../../main/threadNameCtl.cpp
../../corelib/sqlite3Wrapper.cpp

)
include_directories(../../main)
#add_definitions(-DWITHOUT_UPNP )

target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z dl pthread sqlite3)
install(TARGETS mt-test-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

