#cmake_minimum_required (VERSION 2.6)

add_library(core-oscarSecure SHARED
    oscarSecure.cpp
    oscarSecureService.cpp
    oscarSecureUser.cpp
    bufferVerify.h
oscarSecureService.h
oscarSecureUser.h

)

target_link_libraries(core-oscarSecure megatron-core pthread)

install(TARGETS core-oscarSecure DESTINATION ${PLUGIN_TARGET_DIR})

