#include "listenerBase.h"
#include "mutexInspector.h"
ListenerBase* ListenerBase::cast(UnknownBase *c)
{
    
    return (ListenerBase*)c->cast(UnknownCast::listener);
}
ListenerBase::ListenerBase(UnknownBase* i, const std::string& name, const SERVICE_id& sid): listenerName(name),serviceId(sid)
{
    i->addClass(UnknownCast::listener,this);
}

