#-------------------------------------------------

#
# Project created by QtCreator 2015-07-12T21:25:58
#
#-------------------------------------------------

QT       += core gui  svg printsupport
!android{
#QT+= printsupport
}
greaterThan(QT_MAJOR_VERSION, 4): QT += webenginewidgets

TARGET = pq_app
TEMPLATE = app
CONFIG+= c++11
CONFIG+= precompile_header
PRECOMPILED_HEADER  = stable.h


DEFINES+=__FULL__ QT5
DEFINES+= UPNP_STATICLIB NO_GETADDRINFO STATICLIB  USE_JSONASSERT

unix{
#LIBS+=-lzbar
}

win32{
LIBS+=-L../../../dfs_lib/lib/miniupnp-master/miniupnpc -lminiupnpc
LIBS+=-lws2_32 -lIphlpapi -lgdi32 -lkernel32
}

CONFIG(debug, debug|release) {
TARGET = updateConfig-d
DEFINES+=DEBUG _DEBUG 
} else {
TARGET = updateConfig
}

!macx{
#../../../dfs_lib/lib/zint-2.4.2/backend/libzint.a
}

LIBS+=../../../../dfs_lib/lib/miniupnp-master/miniupnpc/libminiupnpc.a

macx{
#    LIBS+=-L ../../../dfs_lib/lib/zint-2.4.2/backend -lzint
    LIBS += -L../../../dfs_lib/lib/openssl-1.1.0 -lcrypto -lssl
}

INCLUDEPATH += ../../../megatron ../../../megatron/corelib ../../../megatron/corelib/events ../../../megatron/main\
        ../../../dfs_lib/lib/openssl-1.1.0/include ../../../megatron/lib/jsoncpp-0.5.0/include ../../events ../../../megatron/modules/sqlite \
        ../../../dfs_lib/lib/miniupnp-master  ../../../megatron/modules/common ../../../megatron/modules/common/events \
        ../../lib ../../common             ../../lib/colordiff        ../../../dfs_lib/lib/qrencode-3.4.2\
        ../../../dfs_lib/lib/lodepng-master

#../../../dfs_lib/lib/zint-2.4.2/backend

win32{
INCLUDEPATH+= ../../../dfs_lib/lib/regex
DEFINES+=__STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS  __STDC_FORMAT_MACROS
LIBS+=-L../../../dfs_lib\lib\regex -lgnurx

}

android {
    LIBS+= -L../../../megatron/lib/libs/$$ANDROID_TARGET_ARCH

} else {
        LIBS+= -L../../../megatron/lib/openssl-1.0.1h
}
unix{
LIBS+=-lssl -lcrypto -ldl -lz  
}

unix{
#LIBS+=-lmysqlclient
}

win32{
LIBS+=-lws2_32 -L../../../dfs_lib/lib/openssl-1.1.0 -lcrypto -lssl -lgdi32 -lz
}

!win32{
#SOURCES+=    ../../../megatron/modules/mysql/mysqlService.cpp \

#HEADERS+=     ../../../megatron/modules/mysql/mysqlService.h \

}
macx{
INCLUDEPATH+= /usr/local/Cellar/jpeg/8d/include
LIBS+=-L/usr/local/lib 
        LIBS+=      /usr/local/Cellar/jpeg/8d/lib/libjpeg.a
}
SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../megatron/corelib/__crc32.cpp \
    ../../../megatron/corelib/_QUERY.cpp \
    ../../../megatron/corelib/sqlite3Wrapper.cpp \
    ../../../megatron/corelib/broadcaster.cpp \
    ../../../megatron/corelib/commonError.cpp \
    ../../../megatron/corelib/configDB.cpp \
    ../../../megatron/corelib/epoll_socket_info.cpp \
    ../../../megatron/corelib/event.cpp \
    ../../../megatron/corelib/eventDeque.cpp \
    ../../../megatron/corelib/httpConnection.cpp \
    ../../../megatron/corelib/Integer.cpp \
    ../../../megatron/corelib/ioBuffer.cpp \
    ../../../megatron/corelib/jpge.cpp \
    ../../../megatron/corelib/listenerBase.cpp \
    ../../../megatron/corelib/listenerBuffered.cpp \
    ../../../megatron/corelib/listenerBuffered1Thread.cpp \
    ../../../megatron/corelib/listenerPolled.cpp \
    ../../../megatron/corelib/listenerSimple.cpp \
    ../../../megatron/corelib/logging.cpp \
    ../../../megatron/corelib/logRemote.cpp \
    ../../../megatron/corelib/msockaddr_in.cpp \
    ../../../megatron/corelib/mtimespec.cpp \
    ../../../megatron/corelib/mutexable.cpp \
    ../../../megatron/corelib/mutexInspector.cpp \
    ../../../megatron/corelib/objectHandler.cpp \
    \
    ../../../megatron/corelib/Rational.cpp \
    ../../../megatron/corelib/route_t.cpp \
    ../../../megatron/corelib/socketsContainer.cpp \
    ../../../megatron/corelib/st_rsa.cpp \
    ../../../megatron/corelib/unknown.cpp \
    ../../../megatron/corelib/url.cpp \
    ../../../megatron/corelib/webDumpable.cpp \
    ../../../megatron/main/CInstance.cpp \
    ../../../megatron/main/configObj.cpp \
    ../../../megatron/main/CUtils.cpp \
    ../../../megatron/main/megatron.cpp \
    ../../../megatron/main/threadNameCtl.cpp \
    ../../../megatron/modules/errorDispatcher/errorDispatcherService.cpp \
    ../../../megatron/modules/http/httpService.cpp \
    ../../../megatron/modules/http/mime_types.cpp \
    ../../../megatron/modules/objectProxy/objectProxyService.cpp \
    ../../../megatron/modules/oscar/oscarService.cpp \
    ../../../megatron/modules/oscarSecure/oscarSecureService.cpp \
    ../../../megatron/modules/oscarSecure/oscarSecureUser.cpp \
    ../../../megatron/modules/rpc/rpcService.cpp \
    ../../../megatron/modules/socket/socketsContainerForSocketIO.cpp \
    ../../../megatron/modules/socket/socketService.cpp \
    ../../../megatron/modules/ssl/ssl_reg.cpp \
    ../../../megatron/modules/ssl/st_rsa_impl.cpp \
    ../../../megatron/modules/telnet/telnetService.cpp \
    ../../../megatron/modules/timer/timerService.cpp \
    ../../../megatron/modules/webHandler/webHandlerService.cpp \
    ../../../megatron/modules/rpc/upnp_tr.c \
    ../../../megatron/modules/sqlite/sqlite3.c\
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_reader.cpp \
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_value.cpp \
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_writer.cpp \
    ../../../megatron/corelib/jsonHandler.cpp \
    ../../../megatron/corelib/js_utils.cpp \
    ../../../megatron/modules/dfsReferrer/dfsReferrerService.cpp \
    ../../../megatron/modules/dfsReferrer/neighbours.cpp \
    ../../../megatron/modules/dfsReferrer/uplinkconnectionstate.cpp \
    ../../../megatron/modules/dfsReferrer/uriReferals.cpp \
    ../../../dfs_lib/lib/qrencode-3.4.2/mask.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/mmask.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/mqrspec.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/qrencode.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/qrspec.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/rscode.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/split.c\
    ../../../dfs_lib/lib/qrencode-3.4.2/qrinput.c \
    ../../../dfs_lib/lib/qrencode-3.4.2/bitstream.c\
    ../../../megatron/corelib/DBH.cpp \
    ../../../megatron/modules/keys/rsa-priv.cpp \
    ../../../megatron/modules/keys/rsa-pub.cpp \
    ../../../megatron/corelib/_bitStream.cpp \
    ../../../megatron/modules/referrerClient/referrerClientService.cpp \
    GuiUCHandler.cpp \
    UCHandler.cpp


android{
SOURCES+=    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/connecthostport.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/igd_desc_parse.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minisoap.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minissdpc.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniupnpc.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniwget.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minixml.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/portlistingparse.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/receivedata.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnpcommands.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnperrors.c \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnpreplyparse.c\

}

win32{
SOURCES2+=    \
    ../../../dfs_lib/lib/regex/regcomp.c \
    ../../../dfs_lib/lib/regex/regex.c \
    ../../../dfs_lib/lib/regex/regex_internal.c \
    ../../../dfs_lib/lib/regex/regexec.c\
}


HEADERS  += mainwindow.h \
    ../../../megatron/corelib/__crc32.h \
    ../../../megatron/corelib/_QUERY.h \
    ../../../megatron/corelib/ACCOUNT_id.h \
    ../../../megatron/corelib/appType.h \
    ../../../megatron/corelib/AUTHCOOKIE_id.h \
    ../../../megatron/corelib/_bitStream.h \
    ../../../megatron/corelib/broadcaster.h \
    ../../../megatron/corelib/bufferVerify.h \
    ../../../megatron/corelib/CHUNK_id.h \
    ../../../megatron/corelib/CircularBuffer.h \
    ../../../megatron/corelib/CLASS_id.h \
    ../../../megatron/corelib/commonError.h \
    ../../../megatron/corelib/compat_win32.h \
    ../../../megatron/corelib/configDB.h \
    ../../../megatron/corelib/CONTAINER.h \
    ../../../megatron/corelib/DB_id.h \
    ../../../megatron/corelib/DBH.h \
    ../../../megatron/corelib/dbvar.h \
    ../../../megatron/corelib/DFS_fd.h \
    ../../../megatron/corelib/e_poll.h \
    ../../../megatron/corelib/epoll_socket_info.h \
    ../../../megatron/corelib/errorDispatcherCodes.h \
    ../../../megatron/corelib/event.h \
    ../../../megatron/corelib/EVENT_id.h \
    ../../../megatron/corelib/eventDeque.h \
    ../../../megatron/corelib/eventEnum.h \
    ../../../megatron/corelib/GlobalCookie_id.h \
    ../../../megatron/corelib/html.h \
    ../../../megatron/corelib/httpConnection.h \
    ../../../megatron/corelib/IConfigDB.h \
    ../../../megatron/corelib/IConfigObj.h \
    ../../../megatron/corelib/ifaces.h \
    ../../../megatron/corelib/IInstance.h \
    ../../../megatron/corelib/ILogger.h \
    ../../../megatron/corelib/Integer.h \
    ../../../megatron/corelib/IObjectProxy.h \
    ../../../megatron/corelib/ioBuffer.h \
    ../../../megatron/corelib/IProgress.h \
    ../../../megatron/corelib/IRPC.h \
    ../../../megatron/corelib/ISSL.h \
    ../../../megatron/corelib/IThreadNameController.h \
    ../../../megatron/corelib/ITranslation.h \
    ../../../megatron/corelib/IUtils.h \
    ../../../megatron/corelib/JAVACOOKIE_id.h \
    ../../../megatron/corelib/jpge.h \
    ../../../megatron/corelib/jsKeys.h \
    ../../../megatron/corelib/listenerBase.h \
    ../../../megatron/corelib/listenerBuffered.h \
    ../../../megatron/corelib/listenerBuffered1Thread.h \
    ../../../megatron/corelib/listenerPolled.h \
    ../../../megatron/corelib/listenerSimple.h \
    ../../../megatron/corelib/logging.h \
    ../../../megatron/corelib/msockaddr_in.h \
    ../../../megatron/corelib/mtimespec.h \
    ../../../megatron/corelib/mutex_inspector_entry.h \
    ../../../megatron/corelib/mutexable.h \
    ../../../megatron/corelib/mutexInspector.h \
    ../../../megatron/corelib/objectHandler.h \
    ../../../megatron/corelib/packClient.h \
    ../../../megatron/corelib/queryResult.h \
    ../../../megatron/corelib/Rational.h \
    ../../../megatron/corelib/REF.h \
    ../../../megatron/corelib/refstring.h \
    ../../../megatron/corelib/route_t.h \
    ../../../megatron/corelib/SERVICE_id.h \
    ../../../megatron/corelib/serviceEnum.h \
    ../../../megatron/corelib/SOCKET_fd.h \
    ../../../megatron/corelib/SOCKET_id.h \
    ../../../megatron/corelib/socketsContainer.h \
    ../../../megatron/corelib/sqlite3Wrapper.h \
    ../../../megatron/corelib/st_FILE.h \
    ../../../megatron/corelib/st_malloc.h \
    ../../../megatron/corelib/st_rsa.h \
    ../../../megatron/corelib/stdafx.h \
    ../../../megatron/corelib/tbl_names.h \
    ../../../megatron/corelib/tools_mt.h \
    ../../../megatron/corelib/unknown.h \
    ../../../megatron/corelib/unknownCastDef.h \
    ../../../megatron/corelib/url.h \
    ../../../megatron/corelib/USER_id.h \
    ../../../megatron/corelib/VERSION_id.h \
    ../../../megatron/corelib/version_mega.h \
    ../../../megatron/corelib/webDumpable.h \
    ../../../megatron/corelib/zstring.h \
    ../../../megatron/main/CInstance.h \
    ../../../megatron/main/configObj.h \
    ../../../megatron/main/CUtils.h \
    ../../../megatron/main/megatron.h \
    ../../../megatron/main/threadNameCtl.h \
    ../../../megatron/modules/errorDispatcher/errorDispatcherService.h \
    ../../../megatron/modules/http/httpService.h \
    ../../../megatron/modules/objectProxy/objectProxyService.h \
    ../../../megatron/modules/oscar/oscarService.h \
    ../../../megatron/modules/oscarSecure/oscarSecureService.h \
    ../../../megatron/modules/oscarSecure/oscarSecureUser.h \
    ../../../megatron/modules/postgres/postgresService.h \
    ../../../megatron/modules/rpc/rpcService.h \
    ../../../megatron/modules/rpc/upnp_tr.h \
    ../../../megatron/modules/socket/socketsContainerForSocketIO.h \
    ../../../megatron/modules/socket/socketService.h \
    ../../../megatron/modules/socket/socketStats.h \
    ../../../megatron/modules/sqlite/sqlite3.h \
    ../../../megatron/modules/sqlite/sqlite3ext.h \
    ../../../megatron/modules/ssl/rsa-pub_512_1387663237.h \
    ../../../megatron/modules/ssl/st_rsa_impl.h \
    ../../../megatron/modules/telnet/arpa/telnet.h \
    ../../../megatron/modules/telnet/telnet.h \
    ../../../megatron/modules/telnet/telnet_keys.h \
    ../../../megatron/modules/telnet/telnetService.h \
    ../../../megatron/modules/timer/timerService.h \
    ../../../megatron/modules/webHandler/webHandlerService.h \
    ../../../megatron/corelib/jsonHandler.h \
    ../../../megatron/corelib/js_utils.h \
    bonus_type.h \
    ../../../megatron/modules/dfsReferrer/dfsReferrerService.h \
    ../../../megatron/modules/dfsReferrer/equipments.h \
    ../../../megatron/modules/dfsReferrer/linkinfo.h \
    ../../../megatron/modules/dfsReferrer/neighbours.h \
    ../../../megatron/modules/dfsReferrer/uplinkconnectionstate.h \
    ../../../megatron/modules/dfsReferrer/uriReferals.h \
    ../../../megatron/modules/dfsReferrer/addrSet.h \
    ../../../megatron/modules/common/publicService.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/bitstream.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/mask.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/mmask.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/mqrspec.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/qrencode.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/qrinput.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/qrspec.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/rscode.h \
    ../../../dfs_lib/lib/qrencode-3.4.2/split.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/connecthostport.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/igd_desc_parse.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minisoap.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minissdpc.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniupnpc.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniupnpcstrings.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniupnpctypes.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/miniwget.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/minixml.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/portlistingparse.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/receivedata.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnpcommands.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnperrors.h \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/upnpreplyparse.h \
    ../../../megatron/Events/System/Net/socket/_________socketEvent.h \
    ../../../megatron/Events/System/Net/socket/Accepted.h \
    ../../../megatron/Events/System/Net/socket/AddToConnectTCP.h \
    ../../../megatron/Events/System/Net/socket/AddToListenTCP.h \
    ../../../megatron/Events/System/Net/socket/Connected.h \
    ../../../megatron/Events/System/Net/socket/ConnectFailed.h \
    ../../../megatron/Events/System/Net/socket/NotifyBindAddress.h \
    ../../../megatron/Events/System/Net/socket/NotifyOutBufferEmpty.h \
    ../../../megatron/Events/System/Net/socket/StreamRead.h \
    ../../../megatron/Events/System/Net/socket/Write.h \
    ../../../megatron/Events/System/Run/startService.h \
    ../../../megatron/Events/Tools/telnet/___telnetEvent.h \
    ../../../megatron/Events/Tools/telnet/CommandEntered.h \
    ../../../megatron/Events/Tools/telnet/RegisterCommand.h \
    ../../../megatron/Events/Tools/telnet/RegisterType.h \
    ../../../megatron/Events/Tools/telnet/Reply.h \
    ../../../megatron/Events/Tools/webHandler/___webHandlerEvent.h \
    ../../../megatron/Events/Tools/webHandler/RegisterDirectory.h \
    ../../../megatron/Events/Tools/webHandler/RegisterHandler.h \
    ../../../megatron/Events/Tools/webHandler/RequestIncoming.h \
    ../../../megatron/Events/System/Net/rpc/___rpcEvent.h \
    ../../../megatron/Events/System/Net/rpc/Accepted.h \
    ../../../megatron/Events/System/Net/rpc/Binded.h \
    ../../../megatron/Events/System/Net/rpc/Connected.h \
    ../../../megatron/Events/System/Net/rpc/ConnectFailed.h \
    ../../../megatron/Events/System/Net/rpc/CsTlPacket.h \
    ../../../megatron/Events/System/Net/rpc/Disaccept.h \
    ../../../megatron/Events/System/Net/rpc/Disaccepted.h \
    ../../../megatron/Events/System/Net/rpc/Disconnect.h \
    ../../../megatron/Events/System/Net/rpc/Disconnected.h \
    ../../../megatron/Events/System/Net/rpc/IncomingOnAcceptor.h \
    ../../../megatron/Events/System/Net/rpc/IncomingOnConnector.h \
    ../../../megatron/Events/System/Net/rpc/NotifyOutBufferEmpty.h \
    ../../../megatron/Events/System/Net/rpc/PassPacket.h \
    ../../../megatron/Events/System/Net/rpc/ProgressNotification.h \
    ../../../megatron/Events/System/Net/rpc/ProgressSetPosition.h \
    ../../../megatron/Events/System/Net/rpc/SendPacket.h \
    ../../../megatron/Events/System/Net/rpc/SubscribeNotifications.h \
    ../../../megatron/Events/System/Net/rpc/UnsubscribeNotifications.h \
    ../../../megatron/Events/System/Net/rpc/UpnpPortMap.h \
    ../../../megatron/Events/System/Net/rpc/UpnpResult.h \
    ../../../megatron/Events/System/Net/oscar/___oscarEvent.h \
    ../../../megatron/Events/System/Net/oscar/Accepted.h \
    ../../../megatron/Events/System/Net/oscar/AddToListenTCP.h \
    ../../../megatron/Events/System/Net/oscar/Connect.h \
    ../../../megatron/Events/System/Net/oscar/Connected.h \
    ../../../megatron/Events/System/Net/oscar/ConnectFailed.h \
    ../../../megatron/Events/System/Net/oscar/NotifyBindAddress.h \
    ../../../megatron/Events/System/Net/oscar/NotifyOutBufferEmpty.h \
    ../../../megatron/Events/System/Net/oscar/PacketOnAcceptor.h \
    ../../../megatron/Events/System/Net/oscar/PacketOnConnector.h \
    ../../../megatron/Events/System/Net/oscar/SendPacket.h \
    ../../../megatron/Events/System/Net/oscar/SocketClosed.h \
    ../../../megatron/Events/System/timer/___timerEvent.h \
    ../../../megatron/Events/System/timer/ResetAlarm.h \
    ../../../megatron/Events/System/timer/SetAlarm.h \
    ../../../megatron/Events/System/timer/SetTimer.h \
    ../../../megatron/Events/System/timer/StopAlarm.h \
    ../../../megatron/Events/System/timer/StopTimer.h \
    ../../../megatron/Events/System/timer/TickAlarm.h \
    ../../../megatron/Events/System/timer/TickTimer.h \
    ../../../megatron/Events/System/Net/jsonRef/___jsonRefEvent.h \
    ../../../megatron/Events/System/Net/jsonRef/Notify.h \
    ../../../megatron/Events/System/Net/jsonRef/PingNoRef.h \
    ../../../megatron/Events/System/Net/jsonRef/PongNoRef.h \
    ../../../megatron/Events/System/Net/jsonRef/JsonREQ.h \
    ../../../megatron/Events/System/Net/jsonRef/SendJsonRSP.h \
    ../../../megatron/Events/System/Net/jsonRef/JsonRSP.h \
    ../../../megatron/Events/System/Net/http/___httpEvent.h \
    ../../../megatron/Events/System/Net/http/DoListen.h \
    ../../../megatron/Events/System/Net/http/GetBindPortsREQ.h \
    ../../../megatron/Events/System/Net/http/GetBindPortsRSP.h \
    ../../../megatron/Events/System/Net/http/RegisterProtocol.h \
    ../../../megatron/Events/System/Net/http/RequestIncoming.h \
    ../../../megatron/Events/DFS/Referrer/___dfsReferrerEvent.h \
    ../../../megatron/Events/DFS/Referrer/AddNeighbour.h \
    ../../../megatron/Events/DFS/Referrer/Elloh.h \
    ../../../megatron/Events/DFS/Referrer/GetAvailabilitiesRSP.h \
    ../../../megatron/Events/DFS/Referrer/GetDataFromUplinkREQ.h \
    ../../../megatron/Events/DFS/Referrer/GetDataFromUplinkRSP.h \
    ../../../megatron/Events/DFS/Referrer/Hello.h \
    ../../../megatron/Events/DFS/Referrer/Noop.h \
    ../../../megatron/Events/DFS/Referrer/NotifyReferrerUplinkIsDisconnected.h \
    ../../../megatron/Events/DFS/Referrer/NotifyReferrerUplinkIsConnected.h \
    ../../../megatron/Events/DFS/Referrer/NotifyUplink.h \
    ../../../megatron/Events/DFS/Referrer/Ping.h \
    ../../../megatron/Events/DFS/Referrer/Pong.h \
    ../../../megatron/Events/DFS/PipeStore/SearchPipeREQ.h \
    ../../../megatron/Events/DFS/PipeStore/SearchPipeRSP.h \
    ../../../megatron/Events/DFS/Referrer/SubscribeNotifications.h \
    ../../../megatron/Events/DFS/Cs/____dfsCsEvent.h \
    ../../../megatron/Events/DFS/Caps/___dfsCapsEvent.h \
    ../../../megatron/Events/Tools/errorDispatcher/___errorDispatcherEvent.h \
    ../../../megatron/Events/Tools/errorDispatcher/NotifySubscriber.h \
    ../../../megatron/Events/Tools/errorDispatcher/SendMessage.h \
    ../../../megatron/Events/Tools/errorDispatcher/Subscribe.h \
    ../../../megatron/Events/Tools/errorDispatcher/Unsubscribe.h \
    ../../../megatron/Events/DFS/Referrer/ToplinkDeliverREQ.h \
    ../../../megatron/Events/DFS/Referrer/ToplinkDeliverRSP.h \
    ../../../megatron/Events/DFS/Referrer/ToplinkBroadcast.h \
    ../../../megatron/modules/common/jsonInfo.h \
    ../../../megatron/modules/common/linkinfo.h \
    ../../../megatron/corelib/linkinfo.h \
    ../../../megatron/modules/dfsCaps/dfsCapsService.h \
    ../../../megatron/corelib/_bitStream.h \
    ../../../megatron/Events/DFS/Caps/RegisterMyRefferrer.h \
    ../../../megatron/Events/DFS/Caps/GetRefferrers.h \
    ../../../megatron/modules/referrerClient/referrerClientService.h \
    ../../../megatron/Events/DFS/Referrer/InitClient.h \
    ../../../megatron/Events/DFS/Referrer/UpdateConfigREQ.h \
    ../../../megatron/Events/DFS/Referrer/UpdateConfigRSP.h \
    GuiUCHandler.h \
    UCHandler.h

FORMS    += mainwindow.ui \


OTHER_FILES += \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/updateminiupnpcstrings.sh \
    ../../../dfs_lib/lib/miniupnp-master/miniupnpc/testminiwget.sh \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
