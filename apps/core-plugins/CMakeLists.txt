cmake_minimum_required (VERSION 2.6)

add_library(core-modules SHARED
core-modules.cpp 
../../modules/http/httpService.cpp
../../modules/http/mime_types.cpp
../../modules/rpc/rpcService.cpp
../../modules/webHandler/webHandlerService.cpp
../../modules/timer/timerService.cpp
../../modules/objectProxy/objectProxyService.cpp
../../modules/errorDispatcher/errorDispatcherService.cpp
../../modules/telnet/telnetService.cpp
../../modules/logger/loggerService.cpp
../../modules/socket/socketService.cpp
../../modules/socket/socketsContainerForSocketIO.cpp
../../modules/oscar/oscarService.cpp
../../modules/oscarSecure/oscarSecureService.cpp
../../modules/oscarSecure/oscarSecureUser.cpp
)
target_link_libraries(core-modules megatron-core)
install(TARGETS core-modules DESTINATION ${PLUGIN_TARGET_DIR})

