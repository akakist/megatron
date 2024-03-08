#ifndef _______WEBHTTTP___SERVICE____EVENT_______H1
#define _______WEBHTTTP___SERVICE____EVENT_______H1
#include "httpConnection.h"
#include "SOCKET_id.h"
#include "event_mt.h"

#include "route_t.h"
#include "json/json.h"

#include "serviceEnum.h"


namespace ServiceEnum
{
    const SERVICE_id WebHandler("WebHandler");
}
namespace webHandlerEventEnum
{

    const EVENT_id RegisterHandler("webHandlerRegisterHandler");
    const EVENT_id RegisterDirectory("webHandlerRegisterDirectory");
    const EVENT_id RequestIncoming("webHandlerRequestIncoming");
}

#endif
