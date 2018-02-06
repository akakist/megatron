#ifndef ___OS_PP_SERVICE_EVENT
#define ___OS_PP_SERVICE_EVENT
#include "eventEnum.h"
#include "serviceEnum.h"
#include "SERVICE_id.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id Oscar(_xs_Oscar);
    const SERVICE_id OscarSecure(_xs_OscarSecure);
}

namespace oscarEventEnum
{

    const EVENT_id SendPacket(_xe_oscarSendPacket);
    const EVENT_id AddToListenTCP(_xe_oscarAddToListenTCP);
    const EVENT_id Connect(_xe_oscarConnect);
    const EVENT_id PacketOnAcceptor(_xe_oscarPacketOnAcceptor);
    const EVENT_id PacketOnConnector(_xe_oscarPacketOnConnector);
    const EVENT_id Connected(_xe_oscarConnected);
    const EVENT_id Accepted(_xe_oscarAccepted);
    const EVENT_id NotifyBindAddress(_xe_oscarNotifyBindAddress);
    const EVENT_id NotifyOutBufferEmpty(_xe_oscar_NotifyOutBufferEmpty);
    const EVENT_id SocketClosed(_xe_oscar_SocketClosed);
    const EVENT_id ConnectFailed(_xe_oscar_ConnectFailed);
}


#endif
