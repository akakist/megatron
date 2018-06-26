
cmake_minimum_required (VERSION 2.6)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()

if(LINUX)
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.11.0/gcc_64)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.11.0/clang_64)
endif()


OPTION(MT_CORE_SERVICES_SINGLE_PLUGING "Build apps/core-plugins - all core plugins in one file" ON)
include_directories(corelib local  lib/jsoncpp-0.5.0/include /usr/local/include .. .  )
#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -g -ggdb3 ")
link_directories( /usr/local/lib)
#link_directories(~/bin/lib /usr/local/lib)
IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
SET(ENABLE_QT_GUI on)


SET(CAPS_IP "localhost")
#SET(CAPS_IP "127.0.0.1")
SET(PORT 9900)
SET(CLIENTPORT 9901)

SET(PROJECT_BASE_DIR ~/bin-${PORT})
SET(PLUGIN_TARGET_DIR ${PROJECT_BASE_DIR}/plugin)
SET(BIN_TARGET_DIR ${PROJECT_BASE_DIR}/bin)
SET(LIB_TARGET_DIR ${PROJECT_BASE_DIR}/lib)
SET(LOG_TARGET_DIR ${PROJECT_BASE_DIR}/log)
SET(DATA_TARGET_DIR ${PROJECT_BASE_DIR}/data)
SET(CONFIG_TARGET_DIR ${PROJECT_BASE_DIR}/conf)
SET(CACHE_TARGET_DIR ${PROJECT_BASE_DIR}/cache)


CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/megatron_config.h.in"
                "${PROJECT_SOURCE_DIR}/megatron_config.h"
                NEWLINE_STYLE UNIX )


set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
add_definitions(-DWITH_IO  -DWITH_ZXING -DWITH_ZBAR -DWITHOUT_UPNP)
#add_definitions(-O3) -DMUTEX_INSPECTOR_DEBUG 
#-DDEBUG

ADD_SUBDIRECTORY(corelib)
ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(apps)
#ADD_SUBDIRECTORY(test)

IF(ENABLE_QT_GUI)
ADD_SUBDIRECTORY(tools/updateConfig)
ENDIF()

#uncomment one of two lines
#    ADD_SUBDIRECTORY(modules/ssl)
#    ADD_SUBDIRECTORY(apps/core-plugins)
    
    ADD_SUBDIRECTORY(modules)
############################


#ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/../utils2)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utils2 ${CMAKE_CURRENT_BINARY_DIR}/utils2)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../discount_system_c  ${CMAKE_CURRENT_BINARY_DIR}/discount_system_c)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../madison  ${CMAKE_CURRENT_BINARY_DIR}/madison)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../guard  ${CMAKE_CURRENT_BINARY_DIR}/guard)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../coop  ${CMAKE_CURRENT_BINARY_DIR}/coop)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../blockchain  ${CMAKE_CURRENT_BINARY_DIR}/blockchain)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../IM  ${CMAKE_CURRENT_BINARY_DIR}/im)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../IMSF  ${CMAKE_CURRENT_BINARY_DIR}/imsf)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../dfs_lib/lib/zxing-cpp/src  ${CMAKE_CURRENT_BINARY_DIR}/dfs_lib/lib/zxing-cpp/src)

