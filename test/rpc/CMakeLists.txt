#cmake_minimum_required (3)

include_directories(../../lib/miniupnp-master)

add_executable(test-rpc2 
mainTest.cpp
main_.cpp
rpcTestService1.cpp
rpcTestService2.cpp

)

target_link_libraries(test-rpc2 megatron-main   zsingle megatron-core ${OPENSSL_CRYPTO_LIBRARY}  z pthread dl)


#install(TARGETS test-rpc2 DESTINATION ${PLUGIN_TARGET_DIR} )
add_test(NAME test-rpc2
         COMMAND test-rpc2)


