#ifndef ____EVT_UdpAssoc__H___111
#define ____EVT_UdpAssoc__H___111
#include "_________socketEvent.h"
#include "IUtils.h"
namespace socketEvent
{
    class UdpAssoc: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        UdpAssoc(
            const SOCKET_id& _socketId4,
            const SOCKET_id& _socketId6,
            const msockaddr_in  &_addr4,
            const msockaddr_in  &_addr6,
            const char* _socketDescription, const route_t & r):
            NoPacked(socketEventEnum::UdpAssoc,r),
            socketId4(_socketId4),
            socketId6(_socketId6),
            addr4(_addr4),
            addr6(_addr6),
            socketDescription(_socketDescription) {}
        const SOCKET_id socketId4;
        const SOCKET_id socketId6;
        const msockaddr_in  addr4;
        const msockaddr_in  addr6;
        const char* socketDescription;
        void jdump(Json::Value &j) const
        {
            j["socketId4"]=std::to_string(CONTAINER(socketId4));
            j["socketId6"]=std::to_string(CONTAINER(socketId6));
            j["addr4"]=addr4.jdump();
            j["addr6"]=addr6.jdump();
            j["socketDescription"]=socketDescription;
        }
    };
    class UdpAssocRSP: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        UdpAssocRSP(
            const SOCKET_id& _socketId4,
            const SOCKET_id& _socketId6,
            const msockaddr_in  &_addr4,
            const msockaddr_in  &_addr6,
            const REF_getter<epoll_socket_info>& _esi4,
            const REF_getter<epoll_socket_info>& _esi6,
            const int& _errcode, const route_t & r):
            NoPacked(socketEventEnum::UdpAssocRSP,r),
            socketId4(_socketId4),
            socketId6(_socketId6),
            addr4(_addr4),
            addr6(_addr6),
            esi4(_esi4),
            esi6(_esi6),
            errcode(_errcode) {}
        const SOCKET_id socketId4;
        const SOCKET_id socketId6;
        const msockaddr_in  addr4;
        const msockaddr_in  addr6;
        const REF_getter<epoll_socket_info> esi4;
        const REF_getter<epoll_socket_info> esi6;
        const int errcode;
        void jdump(Json::Value &j) const
        {
            j["socketId4"]=std::to_string(CONTAINER(socketId4));
            j["socketId6"]=std::to_string(CONTAINER(socketId6));
            j["addr"]=addr4.jdump();
            j["addr"]=addr6.jdump();
            j["errcode"]=errcode;
        }
    };

    class UdpPacketIncoming: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        UdpPacketIncoming(const REF_getter<epoll_socket_info>& _esi,const msockaddr_in  &_addrFrom, const int& _errcode, const std::string& _data, const route_t & r):
            NoPacked(socketEventEnum::UdpPacketIncoming,r),
            esi(_esi),addrFrom(_addrFrom),errcode(_errcode),data(_data) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in  addrFrom;
        const int errcode;
        const std::string data;
        void jdump(Json::Value &j) const
        {
//            j["socketId"]=std::to_string(CONTAINER(socketId));
            j["addrFrom"]=addrFrom.jdump();
            j["errcode"]=errcode;
        }
    };

}


#endif
