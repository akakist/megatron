#ifndef _______TELNETSERVICE____EVENT_______HC
#define _______TELNETSERVICE____EVENT_______HC

#include "___telnetEvent.h"

namespace telnetEvent
{

class RegisterType: public Event::NoPacked
{
    /**
    *   Регистрация типа параметра.
    *   Направление - от клиента
    *   \param name наименование параметра, которое потом будет использоваться в командах.
    *   \param pattern regexp pattern
    *   \param help строка которая будет выводиться в хелпе для типа.
    */
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    RegisterType(const std::string &_name, const std::string& _pattern, const std::string& _help, const route_t& r)
        :NoPacked(telnetEventEnum::RegisterType,"telnetRegisterType",r),name(_name),pattern(_pattern),help(_help) {}

    std::string name,pattern,help;

    void jdump(Json::Value &) const
    {
    }



};



};

#endif
