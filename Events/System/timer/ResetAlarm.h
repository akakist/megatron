#ifndef ____________TIMER_EVENT______H1
#define ____________TIMER_EVENT______H1
#include "___timerEvent.h"
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
#endif
