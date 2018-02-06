#ifndef ____EVT_SOCKET_OPEN__H___111
#define ____EVT_SOCKET_OPEN__H___111
#include "_________socketEvent.h"
namespace socketEvent
{
class AddToConnectTCP: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    AddToConnectTCP(const SOCKET_id& _socketId,const msockaddr_in  &_addr, const std::string& _socketDescription, bool (*_bufferVerify)(const std::string& s),const route_t & r):
        NoPacked(socketEventEnum::AddToConnectTCP,"socketAddToConnectTCP",r),
        socketId(_socketId),addr(_addr),socketDescription(_socketDescription),bufferVerify(_bufferVerify) {}
    const SOCKET_id socketId;
    const msockaddr_in  addr;
    const std::string socketDescription;
    bool (*bufferVerify)(const std::string& s);
    void jdump(Json::Value &) const
    {
    }
};
};


#endif
