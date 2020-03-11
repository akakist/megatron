cmake_minimum_required (VERSION 2.6)

add_executable(mt-test-${PORT}.exe 
mainTest.cpp
)
include_directories(../../main)
if(!ENABLE_QT_GUI)
IF(FreeBSD)
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3)
ELSE()
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 dl)
ENDIF()
else()
IF(FreeBSD)
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 ${Qt5Widgets_LIBRARIES})
ELSE()
target_link_libraries(mt-test-${PORT}.exe megatron-main zsingle megatron-core ssl crypto  z  pthread sqlitecpp sqlite3 dl ${Qt5Widgets_LIBRARIES})
ENDIF()
endif()


install(TARGETS mt-test-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

