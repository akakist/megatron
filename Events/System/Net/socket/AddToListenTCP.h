#ifndef ____EVT_SOCKET_OPEN__H___1234
#define ____EVT_SOCKET_OPEN__H___1234
#include "_________socketEvent.h"
namespace socketEvent
{

    class AddToListenTCP: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        AddToListenTCP(const SOCKET_id& _socketId,const msockaddr_in  &_addr,const char* _socketDescription,const bool& _rebind,const route_t & r):
            NoPacked(socketEventEnum::AddToListenTCP,r),
            socketId(_socketId),addr(_addr),socketDescription(_socketDescription),rebind(_rebind) {}
        const SOCKET_id socketId;
        const msockaddr_in  addr;
        const char* socketDescription;
        const bool rebind;
        void jdump(Json::Value &) const
        {
        }
    };
}


#endif
