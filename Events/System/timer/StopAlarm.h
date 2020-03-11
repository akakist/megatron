#ifndef ____________TIMER_EVENT______H4
#define ____________TIMER_EVENT______H4
#include "___timerEvent.h"
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
#endif
