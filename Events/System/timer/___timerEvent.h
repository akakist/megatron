#ifndef _______________serviceTimerEvent$h
#define _______________serviceTimerEvent$h
#include "event.h"
#include "SERVICE_id.h"

#include "serviceEnum.h"
#include "Real.h"
namespace ServiceEnum
{
    const SERVICE_id Timer("0-Timer");
}
namespace timerEventEnum
{
    const EVENT_id SetTimer("timerSetTimer");
    const EVENT_id SetAlarm("timerSetAlarm");
    const EVENT_id ResetAlarm("timerResetAlarm");
    const EVENT_id StopTimer("timerStopTimer");
    const EVENT_id StopAlarm("timerStopAlarm");
    const EVENT_id TickTimer("timerTickTimer");
    const EVENT_id TickAlarm("timerTickAlarm");
}

#endif
