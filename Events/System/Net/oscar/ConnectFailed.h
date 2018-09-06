#ifndef ______OSCAR_EVENT___H5
#define ______OSCAR_EVENT___H5
#include "___oscarEvent.h"

namespace oscarEvent
{

class ConnectFailed: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    ConnectFailed(const msockaddr_in  &_addr,const route_t & r):
        NoPacked(oscarEventEnum::ConnectFailed,"OscarConnectFailed",r),
        addr(_addr) {}
    const msockaddr_in  addr;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif
