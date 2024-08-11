#pragma once

#include "EVENT_id.h"
#include "IUtils.h"
#include "SERVICE_id.h"
#include "httpConnection.h"
#include "route_t.h"
namespace ServiceEnum
{
    const SERVICE_id HTTP(genum_HTTP);
}
namespace httpEventEnum
{
    const EVENT_id DoListen(genum_httpDoListen);
    const EVENT_id RegisterProtocol(genum_httpRegisterProtocol);
    const EVENT_id GetBindPortsREQ(genum_GetBindPortsREQ);
    const EVENT_id RequestIncoming(genum_httpRequestIncoming);
    const EVENT_id GetBindPortsRSP(genum_GetBindPortsRSP);
}

namespace httpEvent
{
    class RequestIncoming: public Event::NoPacked
    {
        /**
        request is ready, sent by HTTP
        */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RequestIncoming(const REF_getter<HTTP::Request>& __R, const REF_getter<epoll_socket_info>& __esi, const route_t & r)
            :NoPacked(httpEventEnum::RequestIncoming,r),
             req(__R),esi(__esi) {}
        const REF_getter<HTTP::Request> req;
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {
        }

    };


    class RegisterProtocol: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RegisterProtocol(const std::string &_url, const HTTP::IoProtocol &_protocol, const route_t& r)
            :NoPacked(httpEventEnum::RegisterProtocol,r),
             url(_url),protocol(_protocol) {}
        std::string url;
        HTTP::IoProtocol protocol;
        void jdump(Json::Value &) const
        {
        }
    };



    class GetBindPortsREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new GetBindPortsREQ(r);
        }
        GetBindPortsREQ(const route_t& r)
            :Base(httpEventEnum::GetBindPortsREQ,r) {}

        void unpack(inBuffer& )
        {
        }
        void pack(outBuffer&) const
        {
        }
        void jdump(Json::Value &) const
        {
        }
    };
    class GetBindPortsRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new GetBindPortsRSP(r);
        }
        GetBindPortsRSP(const route_t& r)
            :Base(httpEventEnum::GetBindPortsRSP,r) {}
        GetBindPortsRSP(const std::set<msockaddr_in>& _ListenAdr,  const route_t &r)
            :Base(httpEventEnum::GetBindPortsRSP,r),
             listenAddrs(_ListenAdr) {}
        std::set<msockaddr_in> listenAddrs;

        void unpack(inBuffer& o)
        {
            o>>listenAddrs;
        }
        void pack(outBuffer&o) const
        {
            o<<listenAddrs;
        }
        void jdump(Json::Value &v) const
        {
            v["listenAddrs"]=iUtils->dump(listenAddrs);
        }

    };

    class DoListen: public Event::NoPacked
    {
        /**
        request HTTP to listen port
        line like "INADDR_ANY:8080:128" - ip:port:listenBacklog
        */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        DoListen(const msockaddr_in& _addr, const route_t & r)
            :NoPacked(httpEventEnum::DoListen,r),
             addr(_addr) {}
        const msockaddr_in addr;
        void jdump(Json::Value &) const
        {
        }

    };


}

