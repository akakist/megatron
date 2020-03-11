#ifndef _____________REGISTER_HANDRL__HH
#define _____________REGISTER_HANDRL__HH
#include "___webHandlerEvent.h"
namespace webHandlerEvent
{
    class RegisterHandler: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RegisterHandler(const std::string& _url, const std::string& _displayName, const route_t & r)
            :NoPacked(webHandlerEventEnum::RegisterHandler,r),
             url(_url),displayName(_displayName) {}
        const std::string url,displayName;
        void jdump(Json::Value &) const
        {
        }

    };

};

#endif
