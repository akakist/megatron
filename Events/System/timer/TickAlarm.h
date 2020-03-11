#ifndef ____________TIMER_EVENT______H6
#define ____________TIMER_EVENT______H6
#include "___timerEvent.h"
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
#endif
