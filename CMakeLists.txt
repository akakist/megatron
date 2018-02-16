
cmake_minimum_required (VERSION 2.6)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()

if(LINUX)
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.9.1/gcc_64)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.9.1/clang_64)
endif()


OPTION(MT_CORE_SERVICES_SINGLE_PLUGING "Build apps/core-plugins - all core plugins in one file" ON)
include_directories(corelib local  lib/jsoncpp-0.5.0/include /usr/local/include .. .  )
#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -g")
link_directories( /usr/local/lib)
#link_directories(~/bin/lib /usr/local/lib)
IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
SET(ENABLE_QT_GUI on)

SET(CAPS_IP "testquant.com")
#SET(CAPS_IP "127.0.0.1")
SET(PORT 9900)
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


add_definitions(-DWITH_IO  -DWITH_ZXING -DWITH_ZBAR)
#add_definitions(-O3)
#-DDEBUG

ADD_SUBDIRECTORY(corelib)
ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(apps)
ADD_SUBDIRECTORY(test)

IF(ENABLE_QT_GUI)
ADD_SUBDIRECTORY(tools/updateConfig)
ENDIF()

#uncomment one of two lines
#    ADD_SUBDIRECTORY(modules/ssl)
#    ADD_SUBDIRECTORY(apps/core-plugins)
    
    ADD_SUBDIRECTORY(modules)
############################




