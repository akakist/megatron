include_directories(local
			lib/jsoncpp-0.5.0/include 
			.
			corelib
			lib
			lib/miniupnp
			/usr/local/include

)
link_directories( /usr/local/lib)
SET(PORT 2000)

SET(PROJECT_BASE_DIR ~/bin-${PORT})
SET(PLUGIN_TARGET_DIR ${PROJECT_BASE_DIR}/plugin)
SET(BIN_TARGET_DIR ${PROJECT_BASE_DIR}/bin)
SET(LIB_TARGET_DIR ${PROJECT_BASE_DIR}/lib)
SET(LOG_TARGET_DIR ${PROJECT_BASE_DIR}/log)
SET(DATA_TARGET_DIR ${PROJECT_BASE_DIR}/data)
SET(CONFIG_TARGET_DIR ${PROJECT_BASE_DIR}/conf)
SET(CACHE_TARGET_DIR ${PROJECT_BASE_DIR}/cache)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  ")

CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/megatron_config.h.in"
                "${PROJECT_SOURCE_DIR}/megatron_config.h"
                NEWLINE_STYLE UNIX )


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

