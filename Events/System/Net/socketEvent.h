#pragma once

#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "SOCKET_id.h"
#include "epoll_socket_info.h"
#include "event_mt.h"
#include "msockaddr_in.h"
#include "route_t.h"

namespace ServiceEnum
{
    const SERVICE_id Socket("62e6@Socket");
    const SERVICE_id Socks5("ecff@Socks5");
}

namespace socketEventEnum
{

    const EVENT_id AddToListenTCP("56cf@socket.AddToListenTCP");
    const EVENT_id AddToConnectTCP("a4ac@socket.AddToConnectTCP");
    const EVENT_id Accepted("6a2c@socket.Accepted");
    const EVENT_id StreamRead("9a97@socket.StreamRead");
    const EVENT_id Connected("2498@socket.Connected");
    const EVENT_id Disconnected("e9ee@socket.Disconnected");
    const EVENT_id Disaccepted("1f4c@socket.Disaccepted");
    const EVENT_id NotifyBindAddress("dd3a@socket.NotifyBindAddress");
    const EVENT_id NotifyOutBufferEmpty("3e38@socket.NotifyOutBufferEmpty");
    const EVENT_id ConnectFailed("b17f@socket.ConnectFailed");
    const EVENT_id UdpAssoc("283c@socket.UdpAssoc");
    const EVENT_id UdpAssocRSP("86d9@socket.UdpAssocRSP");
    const EVENT_id UdpPacketIncoming("9c70@socket.UdpPacketIncoming");
    const EVENT_id Write("bd2c@socket.Write");
}


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


namespace socketEvent
{
    class StreamRead: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        StreamRead(const REF_getter<epoll_socket_info>  & esi_, const route_t& r)
            :NoPacked(socketEventEnum::StreamRead,r),
             esi(esi_)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;
    };
}



namespace socketEvent
{
    class NotifyOutBufferEmpty: public Event::NoPacked
    {
    public:
        NotifyOutBufferEmpty(const REF_getter<epoll_socket_info> &__S, const route_t& r):
            NoPacked(socketEventEnum::NotifyOutBufferEmpty,r), esi(__S)
        {
        }
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {
        }
    };
}


namespace socketEvent
{
    class NotifyBindAddress: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyBindAddress(const msockaddr_in & __S,const char* _socketDescription,bool _rebind,const route_t&r)
            :NoPacked(socketEventEnum::NotifyBindAddress,r),
             addr(__S),socketDescription(_socketDescription),rebind(_rebind)
        {
        }
        void jdump(Json::Value &j) const
        {
//            j["local_name"]=esi->local_name.jdump();
            j["socketDescription"]=socketDescription;
        }
        const msockaddr_in  addr;
        const char* socketDescription;
        const bool rebind;
    };
}



namespace socketEvent
{
    class Disconnected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disconnected(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(socketEventEnum::Disconnected,r),
             esi(__S),reason(_reason)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;
        const std::string reason;

    };
}



namespace socketEvent
{
    class Disaccepted: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disaccepted(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(socketEventEnum::Disaccepted,r),
             esi(__S),reason(_reason)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;
        const std::string reason;

    };
}



namespace socketEvent
{
    class Connected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Connected(const REF_getter<epoll_socket_info>  & __S,const route_t&r)
            :NoPacked(socketEventEnum::Connected,r),
             esi(__S)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;

    };
}



namespace socketEvent
{
    class ConnectFailed: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        ConnectFailed(const REF_getter<epoll_socket_info>& _esi,const msockaddr_in  &_addr,int _errno_,const route_t & r):
            NoPacked(socketEventEnum::ConnectFailed,r),
            esi(_esi),
            addr(_addr),_errno(_errno_) {}
        REF_getter<epoll_socket_info> esi;
        const msockaddr_in  addr;
        int _errno;
        void jdump(Json::Value &) const
        {
        }
    };

}


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


namespace socketEvent
{
    class AddToConnectTCP: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        AddToConnectTCP(const SOCKET_id& _socketId,const msockaddr_in  &_addr, const char* _socketDescription, const route_t & r):
            NoPacked(socketEventEnum::AddToConnectTCP,r),
            socketId(_socketId),addr(_addr),socketDescription(_socketDescription) {}
        const SOCKET_id socketId;
        const msockaddr_in  addr;
        const char* socketDescription;
        void jdump(Json::Value &j) const
        {
            j["socketId"]=std::to_string(CONTAINER(socketId));
            j["addr"]=addr.jdump();
            j["socketDescription"]=socketDescription;
        }
    };
};


namespace socketEvent {
    class Accepted: public Event::NoPacked
    {
    public:
        Accepted(const REF_getter<epoll_socket_info> &__S, const route_t& r):
            NoPacked(socketEventEnum::Accepted,r), esi(__S)
        {
        }
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {
        }
    };
}

