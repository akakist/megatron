#pragma once
#include "IInstance.h"
#include "IUtils.h"
#include "mutexInspector.h"
#include "Events/System/timerEvent.h"
#include "mutexable.h"
class _timerValues
{
    RWLock lk;
    std::map<int,real> values;
public:
    void set(int k,const real& v)
    {
        WLocker aaa(lk);
        values[k]=v;
    }
    real get(const real& k)
    {
        RLocker dddd(lk);
        std::map<int,real>::iterator i=values.find(k);
        if(i==values.end()) throw CommonError("unkonfigured timer value for %f %s",k,_DMI().c_str());
        return i->second;
    }

};

class TimerHelper
{
    IInstance* ifa;
    _timerValues tw;
public:
    real getTimerValue(int tid)
    {
        return tw.get(tid);
    }
    void setTimerValue(int tid, const real& value)
    {
        tw.set(tid,value);
    }
    TimerHelper(IInstance* _ifa): ifa(_ifa) {}
    void setAlarm(int tid,const REF_getter<refbuffer>&data, const REF_getter<refbuffer>&cookie, const route_t& r)
    {
        ifa->sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(tid,data,cookie,getTimerValue(tid),r));
    }
    void setTimer(int tid,const REF_getter<refbuffer>&data, const REF_getter<refbuffer>&cookie, const route_t& r)
    {
        ifa->sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(tid,data,cookie,getTimerValue(tid),r));
    }
    void resetAlarm(int tid,const REF_getter<refbuffer>&data, const REF_getter<refbuffer>&cookie, const route_t& r)
    {
        ifa->sendEvent(ServiceEnum::Timer,new timerEvent::ResetAlarm(tid,data,cookie,getTimerValue(tid),r));
    }
    void stopAlarm(int tid,const REF_getter<refbuffer>&data, const route_t& r)
    {
        ifa->sendEvent(ServiceEnum::Timer,new timerEvent::StopAlarm(tid,data,r));
    }

};

