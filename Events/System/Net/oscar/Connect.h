#ifndef ______OSCAR_EVENT___H4
#define ______OSCAR_EVENT___H4
#include "___oscarEvent.h"

namespace oscarEvent
{

    class Connect: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Connect(const SOCKET_id& _socketId, const msockaddr_in& _addr,const std::string& _socketDescription,const route_t & r):
            NoPacked(oscarEventEnum::Connect,r),
            socketId(_socketId), addr(_addr),socketDescription(_socketDescription) {}
        const SOCKET_id socketId;
        const msockaddr_in addr;
        const std::string socketDescription;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
