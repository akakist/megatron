#include "uplinkconnectionstate.h"

Json::Value dfsReferrer::_uplinkConnectionState::wdump()
{
    MUTEX_INSPECTOR;
    Json::Value v;
    v["m_isCapsServer"]=m_isCapsServer;
    return v;

}
