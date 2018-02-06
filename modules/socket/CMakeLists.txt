cmake_minimum_required (VERSION 2.6)

add_library(core-socket SHARED
    socket.cpp
    socketService.cpp
    socketsContainerForSocketIO.cpp
)

target_link_libraries(core-socket megatron-core)

install(TARGETS core-socket DESTINATION ${PLUGIN_TARGET_DIR})

