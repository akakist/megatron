#pragma once

#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "epoll_socket_info.h"
#include "event_mt.h"

namespace ServiceEnum
{
    const SERVICE_id RPC("RPC");
    const SERVICE_id ObjectProxyPolled("ObjectProxyPolled");
    const SERVICE_id ObjectProxyThreaded("ObjectProxyThreaded");

}

namespace rpcEventEnum
{

    const EVENT_id PassPacket("rpcPassPacket");
    const EVENT_id SendPacket("rpcSendPacket");
    const EVENT_id SubscribeNotifications("rpcSubscribeNotifications");
    const EVENT_id UnsubscribeNotifications("rpcUnsubscribeNotifications");
    const EVENT_id Disconnected("rpcDisconnected");
    const EVENT_id Disaccepted("rpcDisaccepted");
    const EVENT_id ConnectFailed("rpcConnectFailed");
    const EVENT_id Connected("rpcConnected");
    const EVENT_id Accepted("rpcAccepted");
    const EVENT_id Binded("rpcBinded");
    const EVENT_id IncomingOnAcceptor("rpcIncomingOnAcceptor");
    const EVENT_id IncomingOnConnector("rpcIncomingOnConnector");
    const EVENT_id NotifyOutBufferEmpty("rpc_NotifyOutBufferEmpty");
}




namespace rpcEvent {
    class UnsubscribeNotifications: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        UnsubscribeNotifications(const route_t & r)
            :NoPacked(rpcEventEnum::UnsubscribeNotifications,r)
        {}
        void jdump(Json::Value &) const
        {

        }
    };
}
namespace rpcEvent {
    class SubscribeNotifications: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        SubscribeNotifications(const route_t & r)
            :NoPacked(rpcEventEnum::SubscribeNotifications,r)
        {}
        void jdump(Json::Value &) const
        {

        }
    };
}
namespace rpcEvent {


    class SendPacket: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        SendPacket(const msockaddr_in& _addrTo, const SERVICE_id& dst,const REF_getter<Event::Base>& _ev)
            :NoPacked(rpcEventEnum::SendPacket),
             addressTo(_addrTo),destination(dst), ev(_ev) {}
        const msockaddr_in addressTo;
        const SERVICE_id destination;
        const REF_getter<Event::Base> ev;
        void jdump(Json::Value &j) const
        {
            j["ev"]=ev->dump();
            j["destination"]=destination.dump();
            j["addressTo"]=addressTo.dump();
        }

    };

}
namespace rpcEvent {
    class PassPacket: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        PassPacket(const SOCKET_id& _addrTo, const REF_getter<Event::Base>  &_e)
            :NoPacked(rpcEventEnum::PassPacket),
             socketIdTo(_addrTo),e(_e) {}
        const SOCKET_id socketIdTo;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &j) const
        {
            j["socketIdTo"]=CONTAINER(socketIdTo);
            j["e"]=e->dump();
        }

    };

}
namespace rpcEvent {
    class NotifyOutBufferEmpty: public Event::NoPacked
    {
    public:
        NotifyOutBufferEmpty(const route_t& r)
            :NoPacked(rpcEventEnum::NotifyOutBufferEmpty,r)
        {
        }
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        void jdump(Json::Value &) const
        {
        }
    };

}
namespace rpcEvent {
    class IncomingOnConnector: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        IncomingOnConnector(const REF_getter<epoll_socket_info>&_esi,
                            const msockaddr_in& _addr,const REF_getter<Event::Base> & _e)
            :NoPacked(rpcEventEnum::IncomingOnConnector),
             esi(_esi),connect_addr(_addr),e(_e) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in connect_addr;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &v) const
        {
            v["connect_addr"]=connect_addr.dump();
            v["e"]=e->dump();
        }
    };

}
namespace rpcEvent {

    class IncomingOnAcceptor: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        IncomingOnAcceptor(const REF_getter<epoll_socket_info>&_esi,const REF_getter<Event::Base> & _e)
            :NoPacked(rpcEventEnum::IncomingOnAcceptor),
             esi(_esi),e(_e) {}
        const REF_getter<epoll_socket_info> esi;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &v) const
        {
            v["e"]=e->dump();
        }
    };
}
namespace rpcEvent {

    class Disconnected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disconnected(const REF_getter<epoll_socket_info>& _esi, const msockaddr_in & _addr, const std::string& _reason, const route_t &r)
            :NoPacked(rpcEventEnum::Disconnected,r),
             esi(_esi),destination_addr(_addr), reason(_reason) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in destination_addr;
        const std::string  reason;
        void jdump(Json::Value &) const
        {

        }
    };

}
namespace rpcEvent {

    class Disaccepted: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disaccepted(const REF_getter<epoll_socket_info>& _esi, const std::string& _reason, const route_t &r)
            :NoPacked(rpcEventEnum::Disaccepted,r),
             esi(_esi),reason(_reason) {}
        const REF_getter<epoll_socket_info> esi;
        const std::string  reason;
        void jdump(Json::Value &) const
        {

        }

    };
}
namespace rpcEvent {
    class Connected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Connected(const REF_getter<epoll_socket_info>& _esi, const msockaddr_in & _addr, const route_t &r)
            :NoPacked(rpcEventEnum::Connected,r),
             esi(_esi),addr(_addr) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in addr;
        void jdump(Json::Value &) const
        {

        }

    };

}
namespace rpcEvent {

    class ConnectFailed: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        ConnectFailed(const msockaddr_in & _addr, const route_t &r)
            :NoPacked(rpcEventEnum::ConnectFailed,r),
             destination_addr(_addr) {}
        const msockaddr_in destination_addr;
        void jdump(Json::Value &) const
        {

        }
    };
}
namespace rpcEvent {

    class Binded: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Binded(const REF_getter<epoll_socket_info>& _esi)
            :NoPacked(rpcEventEnum::Binded),
             esi(_esi) {}
        const REF_getter<epoll_socket_info>  esi;
        void jdump(Json::Value &) const
        {

        }
    };
}

namespace rpcEvent {
    class Accepted: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Accepted(const REF_getter<epoll_socket_info>& _esi, const route_t &r)
            :NoPacked(rpcEventEnum::Accepted,r),
             esi(_esi) {}
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {

        }
    };

}
