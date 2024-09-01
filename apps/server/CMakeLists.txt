
add_executable(megatrond 
mainApp.cpp


)
include_directories(../../main)

    IF(FreeBSD)
        target_link_libraries(megatrond megatron-main  megatron-core z  pthread)
    ELSE()
	target_link_libraries(megatrond megatron-main  megatron-core z  pthread dl)
    ENDIF()

install(TARGETS megatrond DESTINATION ${BIN_TARGET_DIR})

