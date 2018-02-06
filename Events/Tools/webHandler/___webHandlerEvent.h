#ifndef _______WEBHTTTP___SERVICE____EVENT_______H1
#define _______WEBHTTTP___SERVICE____EVENT_______H1
#include "httpConnection.h"
#include "SOCKET_id.h"
#include "event.h"

#include "route_t.h"
#include "json/json.h"

#include "serviceEnum.h"
#include "eventEnum.h"

namespace ServiceEnum
{
    const SERVICE_id WebHandler(_xs_WebHandler);
}
namespace webHandlerEventEnum
{

    const EVENT_id RegisterHandler(_xe_webHandlerRegisterHandler);
    const EVENT_id RegisterDirectory(_xe_webHandlerRegisterDirectory);
    const EVENT_id RequestIncoming(_xe_webHandlerRequestIncoming);
}

#endif
