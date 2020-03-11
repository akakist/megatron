#ifndef _______________serviceErrorDispatcherEvent$h
#define _______________serviceErrorDispatcherEvent$h
#include "event.h"
#include "SERVICE_id.h"
#include "serviceEnum.h"

#include "errorDispatcherCodes.h"
namespace ServiceEnum
{
    const SERVICE_id ErrorDispatcher("ErrorDispatcher");

}
namespace errorDispatcherEventEnum
{
    const EVENT_id SendMessage("errorDispatcherSendMessage");
    const EVENT_id Subscribe("errorDispatcherSubscribe");
    const EVENT_id Unsubscribe("errorDispatcherUnsubscribe");
    const EVENT_id NotifySubscriber("errorDispatcherNotifySubscriber");
}


#endif
