cmake_minimum_required (VERSION 3.16)

project (megatron)

if(UNIX AND NOT APPLE)
SET(LINUX true)
endif()

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

FetchContent_Declare(
        OpenSSL
            GIT_REPOSITORY 
            https://github.com/openssl/openssl
            GIT_TAG        master
            USES_TERMINAL_DOWNLOAD TRUE)
FetchContent_MakeAvailable(OpenSSL)
#FIND_PACKAGE(OpenSSL )
# find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR} local)


ADD_SUBDIRECTORY(corelib)
ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(lib)
#IF(!ENABLE_QT_GUI)
    ADD_SUBDIRECTORY(apps)
#ENDIF()
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(modules)


