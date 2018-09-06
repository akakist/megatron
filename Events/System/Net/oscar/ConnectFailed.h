#ifndef ______OSCAR_EVENT___H5
#define ______OSCAR_EVENT___H5
#include "___oscarEvent.h"
#include "epoll_socket_info.h"
namespace oscarEvent
{

    class ConnectFailed: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        ConnectFailed(const REF_getter<epoll_socket_info>& _esi,const msockaddr_in  &_addr,const route_t & r):
            NoPacked(oscarEventEnum::ConnectFailed,r),
            esi(_esi),addr(_addr) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in  addr;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
