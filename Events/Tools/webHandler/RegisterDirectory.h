#ifndef _____________REGISTER_DIR__HH
#define _____________REGISTER_DIR__HH
#include "___webHandlerEvent.h"
namespace webHandlerEvent
{
class RegisterDirectory: public Event::NoPacked
{

public:
    static Base* construct(const route_t &)
    {
        return  NULL;
    }
    RegisterDirectory(const std::string& _url, const std::string& _displayName)
        :NoPacked(webHandlerEventEnum::RegisterDirectory,"webHandlerRegisterDirectory"),
         url(_url),displayName(_displayName) {}
    const std::string url,displayName;
    void jdump(Json::Value &) const
    {
    }

};

};

#endif
