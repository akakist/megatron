#ifndef ____EVT_SOCKET_OPEN__H___111
#define ____EVT_SOCKET_OPEN__H___111
#include "_________socketEvent.h"
#include "IUtils.h"
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
            NoPacked(socketEventEnum::AddToConnectTCP,r),
            socketId(_socketId),addr(_addr),socketDescription(_socketDescription),bufferVerify(_bufferVerify) {}
        const SOCKET_id socketId;
        const msockaddr_in  addr;
        const std::string socketDescription;
        bool (*bufferVerify)(const std::string& s);
        void jdump(Json::Value &j) const
        {
            j["socketId"]=std::to_string(CONTAINER(socketId));
            j["addr"]=addr.jdump();
            j["socketDescription"]=socketDescription;
            j["bufferVerify"]=bufferVerify==NULL?"NULL":"!NULL";
        }
    };
};


#endif
