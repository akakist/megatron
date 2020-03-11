#ifndef ____EVT_SOCKET_OPEN__H___WR
#define ____EVT_SOCKET_OPEN__H___WR

#include "_________socketEvent.h"
namespace socketEvent
{
    class Write: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Write(const SOCKET_id & _socketId,const std::string& _buf)
            :NoPacked(socketEventEnum::Write),
             socketId(_socketId),buf(_buf)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const SOCKET_id socketId;
        const std::string buf;
    };
}


#endif
