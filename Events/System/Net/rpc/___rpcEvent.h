#ifndef ________________RPC___EVENT___H
#define ________________RPC___EVENT___H
#include "eventEnum.h"
#include "serviceEnum.h"
#include "epoll_socket_info.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id RPC(_xs_RPC);
    const SERVICE_id ObjectProxyPolled(_xs_ObjectProxyPolled);
    const SERVICE_id ObjectProxyThreaded(_xs_ObjectProxyThreaded);

}

namespace rpcEventEnum
{

    const EVENT_id PassPacket(_xe_rpcPassPacket);
    const EVENT_id SendPacket(_xe_rpcSendPacket);
    const EVENT_id SubscribeNotifications(_xe_rpcSubscribeNotifications);
    const EVENT_id UnsubscribeNotifications(_xe_rpcUnsubscribeNotifications);
    const EVENT_id Disconnect(_xe_rpcDisconnect);
    const EVENT_id Disaccept(_xe_rpcDisaccept);
    const EVENT_id UpnpPortMap(_xe_rpcPortMap);
    const EVENT_id Disconnected(_xe_rpcDisconnected);
    const EVENT_id Disaccepted(_xe_rpcDisaccepted);
    const EVENT_id ConnectFailed(_xe_rpcConnectFailed);
    const EVENT_id Connected(_xe_rpcConnected);
    const EVENT_id Accepted(_xe_rpcAccepted);
    const EVENT_id Binded(_xe_rpcBinded);
    const EVENT_id IncomingOnAcceptor(_xe_rpcIncomingOnAcceptor);
    const EVENT_id IncomingOnConnector(_xe_rpcIncomingOnConnector);
    const EVENT_id UpnpResult(_xe_rpcUpnpResult);
    const EVENT_id NotifyOutBufferEmpty(_xe_rpc_NotifyOutBufferEmpty);
}




#endif
