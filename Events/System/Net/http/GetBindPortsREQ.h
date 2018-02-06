#ifndef _______HTTTP___SERVICE____EVENT_______H2
#define _______HTTTP___SERVICE____EVENT_______H2

#include "___httpEvent.h"
namespace httpEvent
{

class GetBindPortsREQ: public Event::Base
{
    enum {channel=CHANNEL_100};

public:
    static Base* construct(const route_t &r)
    {
        return new GetBindPortsREQ(r);
    }
    GetBindPortsREQ(const route_t& r)
        :Base(httpEventEnum::GetBindPortsREQ,channel,"GetBindPortsREQ",r) {}

    void unpack(inBuffer& )
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
