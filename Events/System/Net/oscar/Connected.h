#ifndef ______OSCAR_EVENT___H6
#define ______OSCAR_EVENT___H6

#include "___oscarEvent.h"
namespace oscarEvent
{

    class Connected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Connected(const REF_getter<epoll_socket_info>  & __S,const route_t&r)
            :NoPacked(oscarEventEnum::Connected,r),
             esi(__S)
        {
        }
        const REF_getter<epoll_socket_info>  esi;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
