#ifndef _________dfsCapsEvent_h
#define _________dfsCapsEvent_h
#include "eventEnum.h"
#include <string>
#include "SERVICE_id.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
const SERVICE_id DFSCaps("DFSCaps");
}

namespace dfsCapsEventEnum
{
    const EVENT_id RegisterMyRefferrerREQ("RegisterMyRefferrerREQ");
    const EVENT_id GetReferrersREQ("GetReferrersREQ");
    const EVENT_id GetReferrersRSP("GetReferrersRSP");
}
#endif
