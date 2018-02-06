#ifndef _______________serviceErrorDispatcherEvent$h
#define _______________serviceErrorDispatcherEvent$h
#include "event.h"
#include "SERVICE_id.h"
#include "serviceEnum.h"
#include "eventEnum.h"
#include "errorDispatcherCodes.h"
namespace ServiceEnum
{
    const SERVICE_id ErrorDispatcher(_xs_ErrorDispatcher);

}
namespace errorDispatcherEventEnum
{
    const EVENT_id SendMessage(_xe_errorDispatcherSendMessage);
    const EVENT_id Subscribe(_xe_errorDispatcherSubscribe);
    const EVENT_id Unsubscribe(_xe_errorDispatcherUnsubscribe);
    const EVENT_id NotifySubscriber(_xe_errorDispatcherNotifySubscriber);
}


#endif
