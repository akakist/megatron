#cmake_minimum_required(2.8.11)
cmake_minimum_required(VERSION 2.8.11)


#Qt/5.7/clang_64
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})

#find_package(Qt5Multimedia REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})

find_package(Qt5Svg REQUIRED)
include_directories(${Qt5Svg_INCLUDE_DIRS})
add_definitions(${Qt5Svg_DEFINITIONS})

find_package(Qt5WebEngine REQUIRED)
include_directories(${Qt5WebEngine_INCLUDE_DIRS})
add_definitions(${Qt5WebEngine_DEFINITIONS})

if (APPLE) 
      set(CMAKE_MACOSX_RPATH ON) 
      SET(CMAKE_SKIP_BUILD_RPATH TRUE) 
      SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
      SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) 
endif () 




#include_directories(../../common ../../shuv_common ../../../megatron/main ../../lib ../../lib/qcustomplot-source ../../lib/colordiff)
include_directories( ../../../megatron/main)


add_executable(update_config.exe 

#    ../../../megatron/modules/referrerClient/referrerClientService.cpp 

authdialog.cpp
authdialog.ui
main.cpp
mainwindow.cpp
mainwindow.ui

GuiUCHandler.cpp
UCHandler.cpp





)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(update_config.exe ${Qt5Widgets_LIBRARIES} ${Qt5Svg_LIBRARIES} megatron-main   megatron-core qrencode 
    z jpeg sqlite3 dl pthread)
install(TARGETS update_config.exe  DESTINATION ${BIN_TARGET_DIR})
