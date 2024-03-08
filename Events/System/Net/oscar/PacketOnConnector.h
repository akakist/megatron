#ifndef ______OSCAR_EVENT___Ha
#define ______OSCAR_EVENT___Ha
#include "___oscarEvent.h"
#include "event_mt.h"
namespace oscarEvent
{

/// приход буфера на коннекторе
    class PacketOnConnector: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        PacketOnConnector(const REF_getter<epoll_socket_info> & _esi, const REF_getter<refbuffer> &_buf,const route_t &r)
            :NoPacked(oscarEventEnum::PacketOnConnector,r),
             esi(_esi),buf(_buf) {}
        /// сокет
        const REF_getter<epoll_socket_info>  esi;
        /// buffer
        const REF_getter<refbuffer> buf;
        void jdump(Json::Value &) const
        {
        }
    };
}
#endif
