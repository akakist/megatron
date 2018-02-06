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
    AddToListenTCP(const SOCKET_id& _socketId,const msockaddr_in  &_addr,const std::string &_socketDescription,const bool& _rebind,bool (*_bufferVerify)(const std::string& s),const route_t & r):
        NoPacked(socketEventEnum::AddToListenTCP,"socketAddToListenTCP",r),
        socketId(_socketId),addr(_addr),socketDescription(_socketDescription),rebind(_rebind),bufferVerify(_bufferVerify) {}
    const SOCKET_id socketId;
    const msockaddr_in  addr;
    const std::string socketDescription;
    const bool rebind;
    bool (*bufferVerify)(const std::string& s);
    void jdump(Json::Value &) const
    {
    }
};
}


#endif
