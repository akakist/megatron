#ifndef ____________TIMER_EVENT______H5
#define ____________TIMER_EVENT______H5
#include "___timerEvent.h"
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
        :NoPacked(timerEventEnum::StopTimer,"timerStopTimer",r), tid(_id), data(_data)
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
