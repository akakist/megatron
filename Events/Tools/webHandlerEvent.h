#pragma once


#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "epoll_socket_info.h"
#include "event_mt.h"
#include "httpConnection.h"
#include "route_t.h"
namespace ServiceEnum
{
    const SERVICE_id WebHandler("WebHandler");
}
namespace webHandlerEventEnum
{

    const EVENT_id RegisterHandler("webHandlerRegisterHandler");
    const EVENT_id RegisterDirectory("webHandlerRegisterDirectory");
    const EVENT_id RequestIncoming("webHandlerRequestIncoming");
}

namespace webHandlerEvent
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
        RequestIncoming(const REF_getter<HTTP::Request>& __R,
                        const REF_getter<epoll_socket_info>& __esi,const route_t & r)
            :NoPacked(webHandlerEventEnum::RequestIncoming,r),
             req(__R),esi(__esi) {}
        const REF_getter<HTTP::Request> req;
        const REF_getter<epoll_socket_info> esi;
        void jdump(Json::Value &) const
        {
        }

    };

};

namespace webHandlerEvent
{
    class RegisterHandler: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RegisterHandler(const std::string& _url, const std::string& _displayName, const route_t & r)
            :NoPacked(webHandlerEventEnum::RegisterHandler,r),
             url(_url),displayName(_displayName) {}
        const std::string url,displayName;
        void jdump(Json::Value &) const
        {
        }

    };

};

namespace webHandlerEvent
{
    class RegisterDirectory: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return  NULL;
        }
        RegisterDirectory(const std::string& _url, const std::string& _displayName)
            :NoPacked(webHandlerEventEnum::RegisterDirectory),
             url(_url),displayName(_displayName) {}
        const std::string url,displayName;
        void jdump(Json::Value &) const
        {
        }

    };

};

