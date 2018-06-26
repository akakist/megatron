#ifndef ______OSCAR_EVENT___Hb
#define ______OSCAR_EVENT___Hb
#include "___oscarEvent.h"
namespace oscarEvent
{

class SendPacket: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    SendPacket(const SOCKET_id& _sock, const std::string &_buf, const route_t& r)
        :NoPacked(oscarEventEnum::SendPacket,"oscarSendPacket",r),
         socketId(_sock), buf(_buf) {}

    const SOCKET_id socketId;

    /// buffer
    const std::string buf;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif
