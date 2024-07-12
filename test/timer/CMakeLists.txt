

add_executable(test-timer 
___testEvent.h
mainTestTimer.cpp
main_.cpp
testTimerService.cpp
testTimerService.h


)

target_link_libraries(test-timer megatron-main   zsingle megatron-core crypto  z pthread dl)


#install(TARGETS test-timer DESTINATION ${PLUGIN_TARGET_DIR} )
add_test(NAME test-timer
         COMMAND test-timer)


