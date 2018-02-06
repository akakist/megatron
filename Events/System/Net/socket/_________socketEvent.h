#ifndef ____EVT_SOCKET_OPEN__H___
#define ____EVT_SOCKET_OPEN__H___
#include "serviceEnum.h"
#include "eventEnum.h"
#include "epoll_socket_info.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id Socket(_xs_Socket);
}

namespace socketEventEnum
{

    const EVENT_id AddToListenTCP(_xe_socketAddToListenTCP);
    const EVENT_id AddToConnectTCP(_xe_socketAddToConnectTCP);
    const EVENT_id Write(_xe_socket_Write);
    const EVENT_id Accepted(_xe_socketAccepted);
    const EVENT_id StreamRead(_xe_socketStreamRead);
    const EVENT_id Connected(_xe_socketConnected);
    const EVENT_id NotifyBindAddress(_xe_socketNotifyBindAddress);
    const EVENT_id NotifyOutBufferEmpty(_xe_socket_NotifyOutBufferEmpty);
    const EVENT_id ConnectFailed(_xe_socketConnectFailed);
}


#endif
