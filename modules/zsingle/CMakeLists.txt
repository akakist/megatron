cmake_minimum_required (VERSION 2.6)



#include_directories(../local ../lib/jsoncpp-0.5.0/include /usr/local/include .. . )

add_library(zsingle STATIC

../dfsCaps/dfsCapsService.cpp
../dfsCaps/NetTreeRoute.cpp
../dfsReferrer/dfsReferrerService.cpp
../dfsReferrer/neighbours.cpp
../dfsReferrer/uplinkconnectionstate.cpp
../dfsReferrer/uriReferals.cpp
../errorDispatcher/errorDispatcherService.cpp
../http/httpService.cpp
../http/mime_types.cpp
../objectProxy/objectProxyService.cpp
../oscar/oscarService.cpp
../oscarSecure/oscarSecureService.cpp
../oscarSecure/oscarSecureUser.cpp
../referrerClient/referrerClientService.cpp
../rpc/rpcService.cpp
../socket/socketsContainerForSocketIO.cpp
../socket/socketService.cpp
../ssl/ssl_reg.cpp
../ssl/st_rsa_impl.cpp
../telnet/telnetService.cpp
../timer/timerService.cpp
../webHandler/webHandlerService.cpp


)
#install(TARGETS zsingle DESTINATION ${LIB_TARGET_DIR})

