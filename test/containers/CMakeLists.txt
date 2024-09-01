#cmake_minimum_required (VERSION 2.6)



add_executable(test-containers1 
containers_test_service.cpp
main.cpp

)
target_link_libraries(test-containers1 megatron-main megatron-core zsingle ${OPENSSL_CRYPTO_LIBRARY}  z pthread ${DL})


#install(TARGETS test-containers1 DESTINATION ${PLUGIN_TARGET_DIR} )

add_test(NAME test-containers1
         COMMAND test-containers1)


