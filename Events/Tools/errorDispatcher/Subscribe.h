#ifndef _______________serviceErrorDispatcherEvent$h3
#define _______________serviceErrorDispatcherEvent$h3
#include "___errorDispatcherEvent.h"
namespace errorDispatcherEvent
{

class Subscribe: public Event::Base
{
    enum {channel=CHANNEL_100};


public:
    static Base* construct(const route_t &r)
    {
        return new Subscribe(r);
    }
    Subscribe(const route_t&r)
        :Base(errorDispatcherEventEnum::Subscribe,channel,"errorDispatcherSubscribe",r) {}
    void unpack(inBuffer&)
    {
    }
    void pack(outBuffer&) const
    {
    }
    void jdump(Json::Value &) const
    {
    }
};

}
#endif
