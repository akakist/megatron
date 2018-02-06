#ifndef _____________REQ_INCOMGISTER_DIR__HH
#define _____________REQ_INCOMGISTER_DIR__HH
#include "___webHandlerEvent.h"
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
    RequestIncoming(const REF_getter<HTTP::Request>& __R, const REF_getter<epoll_socket_info>& __esi,const route_t & r)
        :NoPacked(webHandlerEventEnum::RequestIncoming,"webHandlerRequestIncoming",r),
         req(__R),esi(__esi) {}
    const REF_getter<HTTP::Request> req;
    const REF_getter<epoll_socket_info> esi;
    void jdump(Json::Value &) const
    {
    }

};

};

#endif
