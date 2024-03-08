#ifndef ______OSCAR_EVENT___Hb
#define ______OSCAR_EVENT___Hb
#include "___oscarEvent.h"
#include "epoll_socket_info.h"
namespace oscarEvent
{

/// послать евент в оскар
    class SendPacket: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        SendPacket(const REF_getter<epoll_socket_info>& _esi, const REF_getter<refbuffer> &_buf, const route_t& r)
            :NoPacked(oscarEventEnum::SendPacket,r),
             esi(_esi), buf(_buf) {}

        /// сокет
        const REF_getter<epoll_socket_info> esi;

        /// buffer
        const REF_getter<refbuffer> buf;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
