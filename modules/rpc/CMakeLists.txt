cmake_minimum_required (VERSION 3.0)

ADD_SUBDIRECTORY(test)

#include_directories(../../lib/miniupnp-master)
add_library(core-rpc SHARED
            rpc.cpp
            rpcService.cpp
	    rpcService.h
#upnp_tr.h
#            upnp_tr.c
           )

target_link_libraries(core-rpc megatron-core pthread)

install(TARGETS core-rpc DESTINATION ${PLUGIN_TARGET_DIR})


