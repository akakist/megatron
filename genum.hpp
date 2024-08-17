#pragma once
enum genum
{
	genum_Timer,
	genum_socket_NotifyOutBufferEmpty,
	genum_oscar_Disconnected,
	genum_JsonREQ,
	genum_timerSetTimer,
	genum_Socket,
	genum_oscar_Connect,
	genum_errorDispatcherNotifySubscriber,
	genum_rpcConnected,
	genum_errorDispatcherUnsubscribe,
	genum_telnetReply,
	genum_teldemo1,
	genum_ptAddTokenREQ,
	genum_Oscar,
	genum_WebHandler,
	genum_oscar_Disaccepted,
	genum_rpcTestService1,
	genum_socket_UdpPacketIncoming,
	genum_DFSCaps_GetCloudRootsRSP,
	genum_DFSReferrer,
	genum_Logger,
	genum_JsonRSP,
	genum_Telnet,
	genum_DFSCaps,
	genum_httpRegisterProtocol,
	genum_oscar_PacketOnConnector,
	genum_telnetRegisterType,
	genum_DFSReferrer_ToplinkDeliverRSP,
	genum_rpcTest,
	genum_rpcDisaccepted,
	genum_DFSReferrer_Pong,
	genum_socket_ConnectFailed,
	genum_teldemo2,
	genum_rpcPassPacket,
	genum_DFSReferrer_Ping,
	genum_rpcDisconnected,
	genum_testTimer,
	genum_jwtServer,
	genum_Ping,
	genum_timerResetAlarm,
	genum_socket_StreamRead,
	genum_httpRequestIncoming,
	genum_DFSReferrerNotifyReferrerUplinkIsConnected,
	genum_telnetCommandEntered,
	genum_socket_NotifyBindAddress,
	genum_pt_AddTaskRSP,
	genum_OscarSecure,
	genum_HTTP,
	genum_AV,
	genum_rpcIncomingOnAcceptor,
	genum_GEOIP,
	genum_ErrorRSP,
	genum_rpcUnsubscribeNotifications,
	genum_SSL,
	genum_DFSReferrerNotifyReferrerDownlinkDisconnected,
	genum_GetUrSinceREQ,
	genum_rpcSendPacket,
	genum_RPC,
	genum_socket_AddToConnectTCP,
	genum_testWebServer,
	genum_DFSCaps_GetCloudRootsREQ,
	genum_jwtServerWeb,
	genum_oscar_SendPacket,
	genum_DFSReferrer_Noop,
	genum_RefferrerClient,
	genum_oscar_ConnectFailed,
	genum_rpcBinded,
	genum_socket_UdpAssocRSP,
	genum_rpcSubscribeNotifications,
	genum_httpDoListen,
	genum_DFSCaps_GetReferrersRSP,
	genum_socket_UdpAssoc,
	genum_oscar_Accepted,
	genum_timerTickTimer,
	genum_DFSCaps_GetReferrersREQ,
	genum_GetUrSinceRSP,
	genum_DFSReferrerUpdateConfigRSP,
	genum_DFSReferrer_SubscribeNotifications,
	genum_ptTokenAddedRSP,
	genum_timerStopTimer,
	genum_rpcAccepted,
	genum_oscar_NotifyOutBufferEmpty,
	genum_GetBindPortsREQ,
	genum_DFSReferrer_ToplinkDeliverREQ,
	genum_telnetRegisterCommand,
	genum_name,
	genum_loggerLogMessage,
	genum_rpcIncomingOnConnector,
	genum_webHandlerRegisterDirectory,
	genum_DFSReferrerUpdateConfigREQ,
	genum_webHandlerRequestIncoming,
	genum_pt_AddTaskREQ,
	genum_rpcConnectFailed,
	genum_rpcTestService2,
	genum_timerStopAlarm,
	genum_prodtestServerWeb,
	genum_errorDispatcherSendMessage,
	genum_oscar_Connected,
	genum_socket_AddToListenTCP,
	genum_timerSetAlarm,
	genum_webHandlerRegisterHandler,
	genum_errorDispatcherSubscribe,
	genum_socket_Disaccepted,
	genum_Socks5,
	genum_Mysql,
	genum_testREQ,
	genum_ptAddTokenRSP,
	genum_DFSCaps_RegisterCloudRoot,
	genum_rpc_NotifyOutBufferEmpty,
	genum_oscar_NotifyBindAddress,
	genum_oscar_AddToListenTCP,
	genum_startService,
	genum_Postgres,
	genum_DFSReferrer_Hello,
	genum_socket_Accepted,
	genum_socket_Disconnected,
	genum_prodtestServer,
	genum_socket_Write,
	genum_DFSReferrerNotifyReferrerUplinkIsDisconnected,
	genum_ObjectProxyThreaded,
	genum_ErrorDispatcher,
	genum_DFSCaps_RegisterMyRefferrerNodeREQ,
	genum_DFSReferrerInitClient,
	genum_oscar_PacketOnAcceptor,
	genum_ObjectProxyPolled,
	genum_timerTickAlarm,
	genum_GetBindPortsRSP,
	genum_socket_Connected,
	genum_testRSP,
};
#include <stdio.h>
inline const char* __gen_string123(int n){
switch(n){
case genum_Timer: return "Timer";
case genum_socket_NotifyOutBufferEmpty: return "socket_NotifyOutBufferEmpty";
case genum_oscar_Disconnected: return "oscar_Disconnected";
case genum_JsonREQ: return "JsonREQ";
case genum_timerSetTimer: return "timerSetTimer";
case genum_Socket: return "Socket";
case genum_oscar_Connect: return "oscar_Connect";
case genum_errorDispatcherNotifySubscriber: return "errorDispatcherNotifySubscriber";
case genum_rpcConnected: return "rpcConnected";
case genum_errorDispatcherUnsubscribe: return "errorDispatcherUnsubscribe";
case genum_telnetReply: return "telnetReply";
case genum_teldemo1: return "teldemo1";
case genum_ptAddTokenREQ: return "ptAddTokenREQ";
case genum_Oscar: return "Oscar";
case genum_WebHandler: return "WebHandler";
case genum_oscar_Disaccepted: return "oscar_Disaccepted";
case genum_rpcTestService1: return "rpcTestService1";
case genum_socket_UdpPacketIncoming: return "socket_UdpPacketIncoming";
case genum_DFSCaps_GetCloudRootsRSP: return "DFSCaps_GetCloudRootsRSP";
case genum_DFSReferrer: return "DFSReferrer";
case genum_Logger: return "Logger";
case genum_JsonRSP: return "JsonRSP";
case genum_Telnet: return "Telnet";
case genum_DFSCaps: return "DFSCaps";
case genum_httpRegisterProtocol: return "httpRegisterProtocol";
case genum_oscar_PacketOnConnector: return "oscar_PacketOnConnector";
case genum_telnetRegisterType: return "telnetRegisterType";
case genum_DFSReferrer_ToplinkDeliverRSP: return "DFSReferrer_ToplinkDeliverRSP";
case genum_rpcTest: return "rpcTest";
case genum_rpcDisaccepted: return "rpcDisaccepted";
case genum_DFSReferrer_Pong: return "DFSReferrer_Pong";
case genum_socket_ConnectFailed: return "socket_ConnectFailed";
case genum_teldemo2: return "teldemo2";
case genum_rpcPassPacket: return "rpcPassPacket";
case genum_DFSReferrer_Ping: return "DFSReferrer_Ping";
case genum_rpcDisconnected: return "rpcDisconnected";
case genum_testTimer: return "testTimer";
case genum_jwtServer: return "jwtServer";
case genum_Ping: return "Ping";
case genum_timerResetAlarm: return "timerResetAlarm";
case genum_socket_StreamRead: return "socket_StreamRead";
case genum_httpRequestIncoming: return "httpRequestIncoming";
case genum_DFSReferrerNotifyReferrerUplinkIsConnected: return "DFSReferrerNotifyReferrerUplinkIsConnected";
case genum_telnetCommandEntered: return "telnetCommandEntered";
case genum_socket_NotifyBindAddress: return "socket_NotifyBindAddress";
case genum_pt_AddTaskRSP: return "pt_AddTaskRSP";
case genum_OscarSecure: return "OscarSecure";
case genum_HTTP: return "HTTP";
case genum_AV: return "AV";
case genum_rpcIncomingOnAcceptor: return "rpcIncomingOnAcceptor";
case genum_GEOIP: return "GEOIP";
case genum_ErrorRSP: return "ErrorRSP";
case genum_rpcUnsubscribeNotifications: return "rpcUnsubscribeNotifications";
case genum_SSL: return "SSL";
case genum_DFSReferrerNotifyReferrerDownlinkDisconnected: return "DFSReferrerNotifyReferrerDownlinkDisconnected";
case genum_GetUrSinceREQ: return "GetUrSinceREQ";
case genum_rpcSendPacket: return "rpcSendPacket";
case genum_RPC: return "RPC";
case genum_socket_AddToConnectTCP: return "socket_AddToConnectTCP";
case genum_testWebServer: return "testWebServer";
case genum_DFSCaps_GetCloudRootsREQ: return "DFSCaps_GetCloudRootsREQ";
case genum_jwtServerWeb: return "jwtServerWeb";
case genum_oscar_SendPacket: return "oscar_SendPacket";
case genum_DFSReferrer_Noop: return "DFSReferrer_Noop";
case genum_RefferrerClient: return "RefferrerClient";
case genum_oscar_ConnectFailed: return "oscar_ConnectFailed";
case genum_rpcBinded: return "rpcBinded";
case genum_socket_UdpAssocRSP: return "socket_UdpAssocRSP";
case genum_rpcSubscribeNotifications: return "rpcSubscribeNotifications";
case genum_httpDoListen: return "httpDoListen";
case genum_DFSCaps_GetReferrersRSP: return "DFSCaps_GetReferrersRSP";
case genum_socket_UdpAssoc: return "socket_UdpAssoc";
case genum_oscar_Accepted: return "oscar_Accepted";
case genum_timerTickTimer: return "timerTickTimer";
case genum_DFSCaps_GetReferrersREQ: return "DFSCaps_GetReferrersREQ";
case genum_GetUrSinceRSP: return "GetUrSinceRSP";
case genum_DFSReferrerUpdateConfigRSP: return "DFSReferrerUpdateConfigRSP";
case genum_DFSReferrer_SubscribeNotifications: return "DFSReferrer_SubscribeNotifications";
case genum_ptTokenAddedRSP: return "ptTokenAddedRSP";
case genum_timerStopTimer: return "timerStopTimer";
case genum_rpcAccepted: return "rpcAccepted";
case genum_oscar_NotifyOutBufferEmpty: return "oscar_NotifyOutBufferEmpty";
case genum_GetBindPortsREQ: return "GetBindPortsREQ";
case genum_DFSReferrer_ToplinkDeliverREQ: return "DFSReferrer_ToplinkDeliverREQ";
case genum_telnetRegisterCommand: return "telnetRegisterCommand";
case genum_name: return "name";
case genum_loggerLogMessage: return "loggerLogMessage";
case genum_rpcIncomingOnConnector: return "rpcIncomingOnConnector";
case genum_webHandlerRegisterDirectory: return "webHandlerRegisterDirectory";
case genum_DFSReferrerUpdateConfigREQ: return "DFSReferrerUpdateConfigREQ";
case genum_webHandlerRequestIncoming: return "webHandlerRequestIncoming";
case genum_pt_AddTaskREQ: return "pt_AddTaskREQ";
case genum_rpcConnectFailed: return "rpcConnectFailed";
case genum_rpcTestService2: return "rpcTestService2";
case genum_timerStopAlarm: return "timerStopAlarm";
case genum_prodtestServerWeb: return "prodtestServerWeb";
case genum_errorDispatcherSendMessage: return "errorDispatcherSendMessage";
case genum_oscar_Connected: return "oscar_Connected";
case genum_socket_AddToListenTCP: return "socket_AddToListenTCP";
case genum_timerSetAlarm: return "timerSetAlarm";
case genum_webHandlerRegisterHandler: return "webHandlerRegisterHandler";
case genum_errorDispatcherSubscribe: return "errorDispatcherSubscribe";
case genum_socket_Disaccepted: return "socket_Disaccepted";
case genum_Socks5: return "Socks5";
case genum_Mysql: return "Mysql";
case genum_testREQ: return "testREQ";
case genum_ptAddTokenRSP: return "ptAddTokenRSP";
case genum_DFSCaps_RegisterCloudRoot: return "DFSCaps_RegisterCloudRoot";
case genum_rpc_NotifyOutBufferEmpty: return "rpc_NotifyOutBufferEmpty";
case genum_oscar_NotifyBindAddress: return "oscar_NotifyBindAddress";
case genum_oscar_AddToListenTCP: return "oscar_AddToListenTCP";
case genum_startService: return "startService";
case genum_Postgres: return "Postgres";
case genum_DFSReferrer_Hello: return "DFSReferrer_Hello";
case genum_socket_Accepted: return "socket_Accepted";
case genum_socket_Disconnected: return "socket_Disconnected";
case genum_prodtestServer: return "prodtestServer";
case genum_socket_Write: return "socket_Write";
case genum_DFSReferrerNotifyReferrerUplinkIsDisconnected: return "DFSReferrerNotifyReferrerUplinkIsDisconnected";
case genum_ObjectProxyThreaded: return "ObjectProxyThreaded";
case genum_ErrorDispatcher: return "ErrorDispatcher";
case genum_DFSCaps_RegisterMyRefferrerNodeREQ: return "DFSCaps_RegisterMyRefferrerNodeREQ";
case genum_DFSReferrerInitClient: return "DFSReferrerInitClient";
case genum_oscar_PacketOnAcceptor: return "oscar_PacketOnAcceptor";
case genum_ObjectProxyPolled: return "ObjectProxyPolled";
case genum_timerTickAlarm: return "timerTickAlarm";
case genum_GetBindPortsRSP: return "GetBindPortsRSP";
case genum_socket_Connected: return "socket_Connected";
case genum_testRSP: return "testRSP";
default: return "undef";
}
return "undef";
}
