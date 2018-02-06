#ifndef _______TELNETSERVICE____EVENT_______H
#define _______TELNETSERVICE____EVENT_______H
#include "SOCKET_id.h"
#include "event.h"

#include "route_t.h"
#include "listenerBase.h"
#include "serviceEnum.h"
#include "eventEnum.h"

namespace ServiceEnum
{
    const SERVICE_id Telnet (_xs_Telnet);

}
namespace telnetEventEnum
{
    const EVENT_id RegisterType(_xe_telnetRegisterType);
    const EVENT_id RegisterCommand(_xe_telnetRegisterCommand);
    const EVENT_id Reply(_xe_telnetReply);
    const EVENT_id CommandEntered(_xe_telnetCommandEntered);
}



#endif
