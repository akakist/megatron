#ifndef ___OS_PP_SERVICE_EVENT
#define ___OS_PP_SERVICE_EVENT

#include "serviceEnum.h"
#include "SERVICE_id.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id Oscar("Oscar");
    const SERVICE_id OscarSecure("OscarSecure");
}

namespace oscarEventEnum
{

    const EVENT_id SendPacket("oscar.SendPacket");
    const EVENT_id AddToListenTCP("oscar.AddToListenTCP");
    const EVENT_id Connect("oscar.Connect");
    const EVENT_id PacketOnAcceptor("oscar.PacketOnAcceptor");
    const EVENT_id PacketOnConnector("oscar.PacketOnConnector");
    const EVENT_id Connected("oscar.Connected");
    const EVENT_id Disconnected("oscar.Disconnected");
    const EVENT_id Accepted("oscar.Accepted");
    const EVENT_id Disaccepted("oscar.Disaccepted");
    const EVENT_id NotifyBindAddress("oscar.NotifyBindAddress");
    const EVENT_id NotifyOutBufferEmpty("oscar.NotifyOutBufferEmpty");
    const EVENT_id ConnectFailed("oscar.ConnectFailed");
}


#endif
