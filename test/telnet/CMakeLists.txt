include_directories( .) 


#ADD_SUBDIRECTORY(server)
ADD_SUBDIRECTORY(service1)

add_executable(test-telnet
mainTestTELNET.cpp
main_.cpp

)

target_link_libraries(test-telnet teldemo1  megatron-main   zsingle megatron-core crypto  z pthread dl)


