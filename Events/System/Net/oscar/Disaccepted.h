#ifndef ______OSCAR_EVENT___H6123
#define ______OSCAR_EVENT___H6123

#include "___oscarEvent.h"
namespace oscarEvent
{

    class Disaccepted: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disaccepted(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(oscarEventEnum::Disaccepted,r),
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
