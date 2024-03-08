
#cmake_minimum_required (VERSION 2.6)
project (dfsCoreModules)

ADD_SUBDIRECTORY(rpc)
ADD_SUBDIRECTORY(udp)

IF(ENABLE_GTEST)
    ADD_SUBDIRECTORY(containers)
ENDIF()