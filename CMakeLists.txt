cmake_minimum_required (VERSION 3.16)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC ")

if(LINUX)
#    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.15.2/gcc_64)
    set(CMAKE_PREFIX_PATH /usr/lib/qt5/bin)
    set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker --export-dynamic -Xlinker -zdefs")
else()
    set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.15.2/clang_64)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

include(CTest)

include(FetchContent)

#FetchContent_Declare(
#        OpenSSL
#            GIT_REPOSITORY 
#            https://github.com/openssl/openssl.git
#            GIT_TAG        master
#            USES_TERMINAL_DOWNLOAD TRUE)
#fetchContent_MakeAvailable(OpenSSL)
#FIND_PACKAGE(OpenSSL )
#find_package(OpenSSL REQUIRED)
set (OPENSSL_INCLUDE_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1t/include) 
set (OPENSSL_LIB_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1t/lib)
link_directories(${OPENSSL_LIB_DIR})
include_directories(${OPENSSL_INCLUDE_DIR} local /usr/local lib/jsoncpp-0.5.0/include . corelib lib/SQLiteCpp lib main lib/sqlite3)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC -O3")


SET(CAPS_IP "x.testquant.com")
SET(PORT 10100)


IF(LINUX)
SET(DL "dl")
ENDIF()

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



ADD_SUBDIRECTORY(corelib)
ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(lib)
#IF(!ENABLE_QT_GUI)
    ADD_SUBDIRECTORY(apps)
#ENDIF()
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(modules)


