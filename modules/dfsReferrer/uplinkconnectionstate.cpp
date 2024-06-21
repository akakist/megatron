#include "uplinkconnectionstate.h"
#include "mutexInspector.h"

Json::Value dfsReferrer::_uplinkConnectionState::wdump()
{
    MUTEX_INSPECTOR;
    Json::Value v;
    v["m_isTopServer"]=m_isTopServer;
    return v;

}
