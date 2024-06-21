#cmake_minimum_required (VERSION 2.6)





add_library(zsingle STATIC
../dfsCaps/CapsAlgorithm.cpp

../dfsCaps/dfsCapsService.cpp
../dfsReferrer/dfsReferrerService.cpp
#../dfsReferrer/neighbours.cpp
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
../rpc/upnp_tr.c
../socket/socketService.cpp
../ssl/ssl_reg.cpp
../ssl/st_rsa_impl.cpp
../telnet/telnetService.cpp
../timer/timerService.cpp
../webHandler/webHandlerService.cpp

../geoip/CapsGeoIP.cpp
../geoip/geoip_impl.cpp
../geoip/geoip_reg.cpp



)
#install(TARGETS zsingle DESTINATION ${LIB_TARGET_DIR})

