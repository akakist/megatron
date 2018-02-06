#ifndef _______HTTTP___SERVICE____EVENT_______H4
#define _______HTTTP___SERVICE____EVENT_______H4

#include "___httpEvent.h"
namespace httpEvent
{
class RegisterProtocol: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    RegisterProtocol(const std::string &_url, const HTTP::IoProtocol &_protocol, const route_t& r)
        :NoPacked(httpEventEnum::RegisterProtocol,"httpRegisterProtocol",r),
         url(_url),protocol(_protocol) {}
    std::string url;
    HTTP::IoProtocol protocol;
    void jdump(Json::Value &) const
    {
    }
};


}

#endif
