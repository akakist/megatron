#ifndef _______TELNETSERVICE____EVENT_______H
#define _______TELNETSERVICE____EVENT_______H
#include "SOCKET_id.h"
#include "event_mt.h"

#include "route_t.h"
#include "listenerBase.h"
#include "serviceEnum.h"


namespace ServiceEnum
{
    const SERVICE_id Telnet ("Telnet");

}
namespace telnetEventEnum
{
    const EVENT_id RegisterType("telnetRegisterType");
    const EVENT_id RegisterCommand("telnetRegisterCommand");
    const EVENT_id Reply("telnetReply");
    const EVENT_id CommandEntered("telnetCommandEntered");
}



#endif
