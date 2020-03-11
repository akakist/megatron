include_directories(local
			lib/jsoncpp-0.5.0/include 
			.
			corelib
			lib
			lib/miniupnp
			/usr/local/include

)
link_directories( /usr/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  ")

add_definitions( -DWITHOUT_UPNP)

ADD_SUBDIRECTORY(corelib)
ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(lib)
#IF(!ENABLE_QT_GUI)
    ADD_SUBDIRECTORY(apps)
#ENDIF()
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(modules)

IF(ENABLE_QT_GUI)
#ADD_SUBDIRECTORY(tools/updateConfig)
ENDIF()

