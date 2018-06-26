#ifndef ______OSCAR_EVENT___H9
#define ______OSCAR_EVENT___H9
#include "___oscarEvent.h"
namespace oscarEvent
{

class PacketOnAcceptor: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    PacketOnAcceptor(const REF_getter<epoll_socket_info> & _esi, const std::string &_buf, const route_t &r)
        :NoPacked(oscarEventEnum::PacketOnAcceptor,"oscarPacketOnAcceptor",r),
         esi(_esi),buf(_buf) {}
    const REF_getter<epoll_socket_info>  esi;
    const std::string buf;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif
