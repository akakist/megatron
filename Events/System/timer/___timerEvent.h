#ifndef _______________serviceTimerEvent$h
#define _______________serviceTimerEvent$h
#include "event.h"
#include "SERVICE_id.h"
#include "eventEnum.h"
#include "serviceEnum.h"
#include "Real.h"
namespace ServiceEnum
{
    const SERVICE_id Timer(_xs_Timer);
}
namespace timerEventEnum
{
    const EVENT_id SetTimer(_xe_timerSetTimer);
    const EVENT_id SetAlarm(_xe_timerSetAlarm);
    const EVENT_id ResetAlarm(_xe_timerResetAlarm);
    const EVENT_id StopTimer(_xe_timerStopTimer);
    const EVENT_id StopAlarm(_xe_timerStopAlarm);
    const EVENT_id TickTimer(_xe_timerTickTimer);
    const EVENT_id TickAlarm(_xe_timerTickAlarm);
}

#endif
