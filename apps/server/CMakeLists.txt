#cmake_minimum_required (VERSION 2.6)

add_executable(mt-main-${PORT}.exe 
mainApp.cpp
#../../main/CInstance.cpp
#../../main/CUtils.cpp
#../../main/configObj.cpp
#../../main/megatron.cpp
#../../main/threadNameCtl.cpp


)
include_directories(../../main)

if(ENABLE_QT_GUI)

    IF(FreeBSD)
        target_link_libraries(mt-main-${PORT}.exe megatron-main  megatron-core z  pthread ${Qt5Widgets_LIBRARIES})
    ELSE()
	target_link_libraries(mt-main-${PORT}.exe megatron-main  megatron-core z  pthread dl ${Qt5Widgets_LIBRARIES})
    ENDIF()
else()
    IF(FreeBSD)
        target_link_libraries(mt-main-${PORT}.exe megatron-main  megatron-core z  pthread)
    ELSE()
	target_link_libraries(mt-main-${PORT}.exe megatron-main  megatron-core z  pthread dl)
    ENDIF()
endif()
install(TARGETS mt-main-${PORT}.exe DESTINATION ${BIN_TARGET_DIR})

