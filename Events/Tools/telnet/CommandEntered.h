#ifndef _______TELNETSERVICE____EVENT_______HA
#define _______TELNETSERVICE____EVENT_______HA

#include "___telnetEvent.h"
namespace telnetEvent
{


class CommandEntered: public Event::NoPacked
{
    /**
    *   Регистрация команды
    *   Направление - от сервера
    *   \param socketId  идентидификатор соединения
    *   \param tokens массив принятых слов
    */
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    CommandEntered(const SOCKET_id& s,const std::deque<std::string> &_tokens, const route_t& r)
        :NoPacked(telnetEventEnum::CommandEntered,"telnetCommandEntered",r),socketId(s),tokens(_tokens) {}

    SOCKET_id socketId;
    std::deque<std::string> tokens;

    void jdump(Json::Value &) const
    {
    }

};



};

#endif
