#ifndef ______OSCAR_EVENT___Ha
#define ______OSCAR_EVENT___Ha
#include "___oscarEvent.h"
namespace oscarEvent
{

class PacketOnConnector: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    PacketOnConnector(const REF_getter<epoll_socket_info> & _esi, const std::string &_buf,const route_t &r)
        :NoPacked(oscarEventEnum::PacketOnConnector,"oscarPacketOnConnector",r),
         esi(_esi),buf(_buf) {}
    const REF_getter<epoll_socket_info>  esi;
    const std::string buf;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif
