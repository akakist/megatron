#ifndef _______________I___RPC___H
#define _______________I___RPC___H
#include "msockaddr_in.h"
#include "unknown.h"
#include "unknownCastDef.h"
#include "mutexInspector.h"

class IRPC
{
    /// interface to access to RPC service
public:
    virtual unsigned short getExternalListenPortMain()=0; // network byte order
    virtual std::set<msockaddr_in> getInternalListenAddrs()=0; // network byte order
    virtual void directHandleEvent(const REF_getter<Event::Base>& e)=0;
    virtual int64_t getTotalSendBufferSize()=0;

    IRPC *cast(UnknownBase *c);
    IRPC(UnknownBase* i);
    virtual ~IRPC() {}

};
inline IRPC *IRPC::cast(UnknownBase *c)
{
    return static_cast<IRPC*>(c->cast(UnknownCast::IRPC));
}
inline IRPC::IRPC(UnknownBase* i)
{
    i->addClass(UnknownCast::IRPC,this);
}

#endif
