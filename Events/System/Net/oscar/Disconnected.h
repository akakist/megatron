#ifndef ______OSCAR_EVENT___H6Z0
#define ______OSCAR_EVENT___H6Z0

#include "___oscarEvent.h"
namespace oscarEvent
{

    class Disconnected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disconnected(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(oscarEventEnum::Disconnected,r),
             esi(__S),reason(_reason)
        {
        }
        const REF_getter<epoll_socket_info>  esi;
        const std::string reason;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
