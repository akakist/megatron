#include "listenerBase.h"
#include "mutexInspector.h"
ListenerBase::ListenerBase(const std::string& name, const SERVICE_id& sid): listenerName(name),serviceId(sid)
{
}

