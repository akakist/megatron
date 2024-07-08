#pragma once
#include "EVENT_id.h"
#include "Real.h"
#include "SERVICE_id.h"
#include "event_mt.h"

namespace ServiceEnum
{
    const SERVICE_id Timer("4941@Timer");
}

namespace timerEventEnum
{
    const EVENT_id SetTimer("780e@timerSetTimer");
    const EVENT_id SetAlarm("13d4@timerSetAlarm");
    const EVENT_id ResetAlarm("74f6@timerResetAlarm");
    const EVENT_id StopTimer("fbcc@timerStopTimer");
    const EVENT_id StopAlarm("e1bd@timerStopAlarm");
    const EVENT_id TickTimer("738a@timerTickTimer");
    const EVENT_id TickAlarm("a57e@timerTickAlarm");
}

namespace timerEvent
{

    class TickTimer: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer> data;
        REF_getter<refbuffer> cookie;
        TickTimer(const int& _tid,const REF_getter<refbuffer>& _data,const REF_getter<refbuffer>& _cookie, const route_t & r):NoPacked(timerEventEnum::TickTimer,r),
            tid(_tid),data(_data),cookie(_cookie)
        {
            if(!data.valid())
                data=new refbuffer;
            if(!cookie.valid())
                cookie=new refbuffer;

        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{

    class TickAlarm: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer> data;
        REF_getter<refbuffer> cookie;
        TickAlarm(const int& _tid,const REF_getter<refbuffer>& _data,const REF_getter<refbuffer>& _cookie, const route_t & r):NoPacked(timerEventEnum::TickAlarm,r),
            tid(_tid),data(_data),cookie(_cookie)
        {
            if(!data.valid())
                data=new refbuffer;
            if(!cookie.valid())
                cookie=new refbuffer;

        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{

    class StopTimer:public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer>data;
        StopTimer(const int _id, const REF_getter<refbuffer>& _data,const route_t &r)
            :NoPacked(timerEventEnum::StopTimer,r), tid(_id), data(_data)
        {
            if(!data.valid())
                data=new refbuffer;
        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{

    class StopAlarm:public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        int tid;
        REF_getter<refbuffer> data;
        StopAlarm(const int& _id, const REF_getter<refbuffer>& _data,const route_t &r)
            :NoPacked(timerEventEnum::StopAlarm,r), tid(_id), data(_data)
        {
            if(!data.valid())
                data=new refbuffer;
        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{

    class SetTimer:public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer> data;
        REF_getter<refbuffer> cookie;
        const real delay_secs;
        SetTimer(const int &_id, const REF_getter<refbuffer>& _data, const REF_getter<refbuffer>& _cookie, const real&  _delay_secs, const route_t& r)
            :NoPacked(timerEventEnum::SetTimer,r),
             tid(_id),data(_data),cookie(_cookie),
             delay_secs(_delay_secs)
        {
            if(!data.valid())
                data=new refbuffer;
            if(!cookie.valid())
                cookie=new refbuffer;

        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{
    class SetAlarm:public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer> data;
        REF_getter<refbuffer> cookie;
        const real delay_secs;
        SetAlarm(const int &_id, const REF_getter<refbuffer>& _data, const REF_getter<refbuffer>& _cookie, const real&  _delay_secs, const route_t& r)
            :NoPacked(timerEventEnum::SetAlarm,r),
             tid(_id),data(_data),cookie(_cookie),
             delay_secs(_delay_secs)
        {
            if(!data.valid())
                data=new refbuffer;
            if(!cookie.valid())
                cookie=new refbuffer;

        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
namespace timerEvent
{

    class ResetAlarm:public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const int tid;
        REF_getter<refbuffer> data;
        REF_getter<refbuffer> cookie;
        const real delay_secs;
        ResetAlarm(const int &_id, const REF_getter<refbuffer>& _data, const REF_getter<refbuffer>& _cookie, const real&  _delay_secs, const route_t& r)
            :NoPacked(timerEventEnum::ResetAlarm,r),
             tid(_id),data(_data),cookie(_cookie),
             delay_secs(_delay_secs)
        {
            if(!data.valid())
                data=new refbuffer;
            if(!cookie.valid())
                cookie=new refbuffer;

        }
        void jdump(Json::Value &v) const
        {
            v["tid"]=tid;
        }
    };
}
