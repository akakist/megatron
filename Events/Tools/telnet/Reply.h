#ifndef _______TELNETSERVICE____EVENT_______HD
#define _______TELNETSERVICE____EVENT_______HD

#include "___telnetEvent.h"

namespace telnetEvent
{

class Reply: public Event::NoPacked
{
public:
    /**
    *   Регистрация команды
    *   Направление - от клиента
    *   \param socketId  идентидификатор соединения
    *   \param buffer данные, которые нужно вывести в консоли
    */
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Reply(const SOCKET_id& s,const std::string &_buffer, const route_t& r)
        :NoPacked(telnetEventEnum::Reply,"telnetReply",r),socketId(s),buffer(_buffer) {}
    SOCKET_id socketId;
    std::string buffer;
    void jdump(Json::Value &) const
    {
    }

};



};

#endif
