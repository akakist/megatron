#ifndef ____EVT_SOCKET_OPEN__H___
#define ____EVT_SOCKET_OPEN__H___
#include "serviceEnum.h"

#include "epoll_socket_info.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id Socket("Socket");
    const SERVICE_id Socks5("Socks5");
}

namespace socketEventEnum
{

    const EVENT_id AddToListenTCP("socket.AddToListenTCP");
    const EVENT_id AddToConnectTCP("socket.AddToConnectTCP");
    const EVENT_id Accepted("socket.Accepted");
    const EVENT_id StreamRead("socket.StreamRead");
    const EVENT_id Connected("socket.Connected");
    const EVENT_id Disconnected("socket.Disconnected");
    const EVENT_id Disaccepted("socket.Disaccepted");
    const EVENT_id NotifyBindAddress("socket.NotifyBindAddress");
    const EVENT_id NotifyOutBufferEmpty("socket.NotifyOutBufferEmpty");
    const EVENT_id ConnectFailed("socket.ConnectFailed");
    const EVENT_id UdpAssoc("socket.UdpAssoc");
    const EVENT_id UdpAssocRSP("socket.UdpAssocRSP");
    const EVENT_id UdpPacketIncoming("socket.UdpPacketIncoming");
    const EVENT_id Write("socket.Write");
}


#endif
