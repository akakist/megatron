#ifndef _______HTTTP___SERVICE____EVENT_______H1
#define _______HTTTP___SERVICE____EVENT_______H1
#include "httpConnection.h"
#include "SOCKET_id.h"
#include "event_mt.h"

#include "route_t.h"
#include "json/json.h"
#include "serviceEnum.h"

namespace ServiceEnum
{
    const SERVICE_id HTTP("HTTP");
}
namespace httpEventEnum
{
    const EVENT_id DoListen("httpDoListen");
    const EVENT_id RegisterProtocol("httpRegisterProtocol");
    const EVENT_id GetBindPortsREQ("GetBindPortsREQ");
    const EVENT_id RequestIncoming("httpRequestIncoming");
    const EVENT_id GetBindPortsRSP("GetBindPortsRSP");
}

#endif
