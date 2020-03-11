#ifndef ________________RPC___EVENT___H
#define ________________RPC___EVENT___H

#include "serviceEnum.h"
#include "epoll_socket_info.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id RPC("RPC");
    const SERVICE_id ObjectProxyPolled("ObjectProxyPolled");
    const SERVICE_id ObjectProxyThreaded("ObjectProxyThreaded");

}

namespace rpcEventEnum
{

    const EVENT_id PassPacket("rpcPassPacket");
    const EVENT_id SendPacket("rpcSendPacket");
    const EVENT_id SubscribeNotifications("rpcSubscribeNotifications");
    const EVENT_id UnsubscribeNotifications("rpcUnsubscribeNotifications");
    const EVENT_id UpnpPortMap("rpcPortMap");
    const EVENT_id Disconnected("rpcDisconnected");
    const EVENT_id Disaccepted("rpcDisaccepted");
    const EVENT_id ConnectFailed("rpcConnectFailed");
    const EVENT_id Connected("rpcConnected");
    const EVENT_id Accepted("rpcAccepted");
    const EVENT_id Binded("rpcBinded");
    const EVENT_id IncomingOnAcceptor("rpcIncomingOnAcceptor");
    const EVENT_id IncomingOnConnector("rpcIncomingOnConnector");
    const EVENT_id UpnpResult("rpcUpnpResult");
    const EVENT_id NotifyOutBufferEmpty("rpc_NotifyOutBufferEmpty");
}




#endif
