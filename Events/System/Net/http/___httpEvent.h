#ifndef _______HTTTP___SERVICE____EVENT_______H1
#define _______HTTTP___SERVICE____EVENT_______H1
#include "httpConnection.h"
#include "SOCKET_id.h"
#include "event.h"

#include "route_t.h"
#include "json/json.h"
#include "serviceEnum.h"
#include "eventEnum.h"
namespace ServiceEnum
{
    const SERVICE_id HTTP(_xs_HTTP);
}
namespace httpEventEnum
{
    const EVENT_id DoListen(_xe_httpDoListen);
    const EVENT_id RegisterProtocol(_xe_httpRegisterProtocol);
    const EVENT_id GetBindPortsREQ(_xe_GetBindPortsREQ);
    const EVENT_id RequestIncoming(_xe_httpRequestIncoming);
    const EVENT_id GetBindPortsRSP(_xe_GetBindPortsRSP);
}

#endif
